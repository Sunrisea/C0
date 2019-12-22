
#include <vector>
#include <optional>
#include <utility>
#include <map>
#include <cstdint>
#include <cstddef>
#include <iostream>


namespace miniplc0{
    struct sign
    {
        int index=0;
        int name_index=0;
        std::string name;
        int type=0;//0为常量，1为变量，2为函数
        int address=0;
        int _initialization=0;

        //函数相关，仅当type=2时有效；
        int backtype=0;//返回值类型，0为void 1为int;
        int num_parameters=0;//
    };
    
    class symboltable {
        public:
            symboltable()
            :_constanttable({}),_functiontable({}),_signtable({}),_indextable({}),_index(0),_program_num(0){}
            int init();
            int isvariabledeclared(const std::string& var);
            int isfunctionvariabledeclared(const std::string& var);
            void addvariable(struct sign _sign);
            void addfunction(struct sign _sign);
            void editfuncitonparamNum(const std::string& funcname,int num);
            std::pair<int32_t,int32_t> getvariable(const std::string& name);
            std::vector<sign> getsigntable();
            std::vector<sign> getconstanttable();
            std::vector<sign> getfunctiontable();
            std::vector<int> getindextable();
            std::pair<std::optional<sign>,int> getfunction(std::string& name);
            std::pair<std::optional<sign>,int> getvariable2(std::string& name);
            void initialize(std::string& name);
            int checkmain();

        public:
            std::vector<sign> _constanttable;
            std::vector<sign> _functiontable;
            std::vector<sign> _signtable;
            std::vector<int> _indextable;
            int _index;
            int _program_num;
    };
}




