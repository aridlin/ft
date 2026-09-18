#define FT_IMPLEMENTATION
#ifndef FT_TEST_HEADER
#define FT_TEST_HEADER "../ft.hpp"
#endif
#include FT_TEST_HEADER
#include <thread>
#ifndef _WIN32
#include <sys/wait.h>
#include <signal.h>
#endif

static int failures = 0;
static void check(bool ok, const char* message) {
    if (!ok) { std::fprintf(stderr,"FAIL: %s\n",message); ++failures; }
}
static void post(const char* label, const std::string& value) {
    auto& c=ftht::internal::ctx(); c.out.clear(); c.params.clear(); c.disabled_depth=0;
    c.params.push_back({ftht::internal::widget_id(label),value});
}
static void widgets() {
    for (const char* bad : {"", "oops", "1x", "999999999999999999999999999", "-1"}) {
        const char* items[]={"A","B","C"}; int selected=2;
        post("Choice",bad);
        check(!ftht::dropdown("Choice",items,3,&selected) && selected==2,"dropdown rejects malformed/out-of-range index");
        post("Choice",bad); selected=2;
        check(!ftht::listbox("Choice",items,3,&selected) && selected==2,"listbox rejects malformed/out-of-range index");
        post("Choice",bad); selected=2;
        check(!ftht::radio_group("Choice",items,3,&selected) && selected==2,"radio rejects malformed/out-of-range index");
        post("tabs##A##B##C",bad); selected=2;
        check(!ftht::tabs(items,3,&selected) && selected==2,"tabs reject malformed/out-of-range index");
    }
    for (const char* bad : {"", "oops", "0.2x", "nan", "inf", "1e9999"}) {
        post("Volume",bad);float volume=.5f;
        check(!ftht::slider_float("Volume",&volume,0,1) && volume==.5f,"slider ignores invalid/nonfinite submissions");
    }
    post("Volume","1.5");float volume=.5f;
    check(ftht::slider_float("Volume",&volume,0,1) && volume==1,"slider clamps valid numeric values");
    char short_text[4]="abc";
    post("Name","abcdef");
    check(!ftht::input("Name",short_text,sizeof(short_text)),"truncation to unchanged input is not a change");
    post("Notes","abcdef");
    check(!ftht::text_area("Notes",short_text,sizeof(short_text)),"truncation to unchanged textarea is not a change");
    char utf8[5]="";
    post("Name",u8"a😀");ftht::input("Name",utf8,sizeof(utf8));
    check(std::string(utf8)=="a","input truncates at a UTF-8 boundary");
    post("Notes",u8"a😀");ftht::text_area("Notes",utf8,sizeof(utf8));
    check(std::string(utf8)=="a","textarea truncates at a UTF-8 boundary");
    char exact[6]="";
    post("Name",u8"a😀");check(ftht::input("Name",exact,sizeof(exact)) && std::string(exact)==u8"a😀","exact-capacity UTF-8 text is preserved");
    post("Name","xyz");ftht::begin_disabled();
    check(!ftht::input("Name",short_text,sizeof(short_text)) && std::string(short_text)=="abc","disabled inputs ignore submitted values");ftht::end_disabled();
    post("Name","xyz");
    check(!ftht::input("Name",short_text,sizeof(short_text),ftht::InputFlags::ReadOnly),"read-only inputs ignore submissions");
}
#ifndef _WIN32
static void requests() {
    auto read=[](std::string headers,std::string body,int limit,bool delayed,std::string& raw) {
        int sockets[2]; if(socketpair(AF_UNIX,SOCK_STREAM,0,sockets)!=0) {std::perror("socketpair");std::exit(2);}
        auto& c=ftht::internal::ctx();c.net.client=sockets[0];c.cfg.read_timeout_ms=100;
        std::thread writer([&]{
            if(delayed) {
                send(sockets[1],headers.data(),headers.size(),MSG_NOSIGNAL);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                send(sockets[1],body.data(),body.size(),MSG_NOSIGNAL);
            } else {std::string all=headers+body;send(sockets[1],all.data(),all.size(),MSG_NOSIGNAL);}
            shutdown(sockets[1],SHUT_WR);
        });
        bool ok=ftht::internal::read_http_request(raw,limit);writer.join();
        close(sockets[0]);close(sockets[1]);c.net.client=ftht::internal::invalid_socket;return ok;
    };
    std::string raw;
    check(read("POST / HTTP/1.1\r\nCoNtEnT-LeNgTh: 3\r\n\r\n","a=1",256,true,raw) && raw.substr(raw.size()-3)=="a=1","mixed-case Content-Length reads the complete body");
    check(!read("POST / HTTP/1.1\r\nContent-Length: 999\r\n\r\n",std::string(100,'x'),80,false,raw) && raw.size()<=80,"oversized request is rejected within its byte limit");
    check(!read("POST / HTTP/1.1\r\nContent-Length: 5\r\n\r\n","abc",256,false,raw),"incomplete body is rejected");
    for(const char* length:{"-1","nope","3xyz","999999999999999999999999999"})
        check(!read(std::string("POST / HTTP/1.1\r\nContent-Length: ")+length+"\r\n\r\n","abc",256,false,raw),"invalid Content-Length is rejected");
    check(!read("POST / HTTP/1.1\r\nContent-Length: 3\r\nContent-Length: 5\r\n\r\n","abcde",256,false,raw),"conflicting lengths are rejected");
    check(read("POST / HTTP/1.1\r\nContent-Length: 3\r\nContent-Length: 3\r\n\r\n","abc",256,false,raw),"identical duplicate lengths are accepted");
    check(!read("POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n","0\r\n\r\n",256,false,raw),"unsupported chunked framing is not misread as a form");
    const std::string header="POST / HTTP/1.1\r\nContent-Length: 3\r\n\r\n";
    check(read(header,"abcEXTRA",256,false,raw) && raw==header+"abc","extra bytes are not included in the form body");
    check(read(header,"abc",int(header.size()+3),false,raw),"request exactly at byte limit succeeds");
    check(read("GET / HTTP/1.1\r\nX-Content-Length: 90\r\n\r\n","",256,false,raw),"header-name substrings do not change framing");
    pid_t pid=fork();
    if(pid==0) {
        signal(SIGPIPE,SIG_DFL);int sockets[2];if(socketpair(AF_UNIX,SOCK_STREAM,0,sockets)!=0)_exit(2);
        close(sockets[1]);bool ok=ftht::internal::send_all(sockets[0],"response",8);close(sockets[0]);_exit(ok?1:0);
    }
    check(pid>0,"fork succeeds");
    if(pid>0){int status=0;waitpid(pid,&status,0);check(WIFEXITED(status)&&WEXITSTATUS(status)==0,"disconnected client does not terminate the server with SIGPIPE");}
}
#endif
static void modals() {
    auto& c=ftht::internal::ctx(); c.params.clear(); c.out="prefix";
    ftht::open_modal("Confirm");
    ftht::modal("Confirm", [] { ftht::text("Closing"); ftht::close_modal(); });
    check(c.out=="prefix" && c.modal_label.empty(),"closing a modal removes its markup in the current response");
    c.out.clear();ftht::open_modal("First");
    ftht::modal("First", [] { ftht::open_modal("Second"); });
    check(c.out.empty() && !c.modal_rendered && c.modal_label=="Second","switching modals does not leave the old overlay in the response");
    c.params={{"_ftht_action","_ftht_close_modal"}};c.client_update=true;
    ftht::begin();
    check(c.modal_label.empty(),"built-in fallback modal close works without a matching modal call");
    c.params.clear();c.out.clear();c.client_update=false;
}
static void fixture() {
    auto& c=ftht::internal::ctx();c.out="<!doctype html><html><body><main><form></form></main>";c.toasts.clear();
    ftht::Toast t;t.message="Initial toast";t.duration_ms=1000;ftht::toast(t);
    ftht::internal::append_toast_stack();ftht::internal::append_toast_script();c.out+="</body></html>";
    std::ofstream("build/toast-regression.html")<<c.out;
}
int main(int argc,char** argv) {
    widgets();modals();fixture();
#ifndef _WIN32
    if(argc>1 && std::strcmp(argv[1],"--sockets")==0)requests();
#else
    (void)argc;(void)argv;
#endif
    std::printf("Polish regressions: %d failure(s).\n",failures);return failures?1:0;
}
