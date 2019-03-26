#include <iostream>
#include <string>
#include <unordered_map>

struct my_struct_t
{
  int x;
};

typedef std::unordered_map<std::string, my_struct_t *> react_map_t;
 
int main()
{

    

    std::unordered_map<std::string, int> months;
    

    months["january"] = 31;
    months["february"] = 28;
    months["march"] = 31;
    months["april"] = 30;
    months["may"] = 31;
    months["june"] = 30;
    months["july"] = 31;
    months["august"] = 31;
    months["september"] = 30;
    months["october"] = 31;
    months["november"] = 30;
    months["december"] = 31;

    const char *sep = "september";
    printf("september: %d\n", months[sep]);
    std::cout << "september -> " << months["september"] << std::endl;
    std::cout << "april     -> " << months["april"] << std::endl;
    std::cout << "december  -> " << months["december"] << std::endl;
    std::cout << "february  -> " << months["february"] << std::endl;
    std::cout << "xebruary  -> " << months["xebruary"] << std::endl;

    my_struct_t *p;
    react_map_t rmap;

    p = new my_struct_t;
    p->x = 5;
    rmap["five"] = p;
   
    p = new my_struct_t;
    p->x = 2;
    rmap["two"] = p;


    p = rmap["wrong"];
    printf("p = %p\n", p);
    p = rmap["five"];
    printf("p = %p, n = %d\n", p, p->x);
    p = NULL;
    printf("p = %p\n", p);

    return 0;
}
