#include <cstdint>
#include <cstring>
#include <filesystem>

#ifdef _WIN32
    #error "Windows compilation is unavailable now"
#endif
#ifdef _android_
    #error "Android is unavailable now"
#endif
extern bool Help();
extern uint8_t Install(int argc, char* argv[]);
extern uint8_t Remove(int argc, char* argv[]);
extern uint8_t List(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    if(!std::filesystem::is_directory((std::string)VAR_PATH + "/packages/"))
        std::filesystem::create_directories((std::string)VAR_PATH + "/packages/");

    if(argc < 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) return Help();
    if(!strcmp(argv[1], "-i")) return(Install(argc, argv));
    if(!strcmp(argv[1], "-r")) return(Remove(argc, argv));
    if(!strcmp(argv[1], "-l")) return(List(argc, argv));
    return 0;
}