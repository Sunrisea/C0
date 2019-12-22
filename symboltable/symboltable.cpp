
#include "symboltable.h"


namespace miniplc0{

    int symboltable::init(){
        _indextable.push_back(0);
        return 0;
    }
    //-1时表示变量没有被声明不然表示被声明了
    int symboltable::isvariabledeclared(const std::string& var){
        unsigned long long int i;
        for(i=0;i<_functiontable.size();i++){
            if(_functiontable[i].name==var){
                return 1;
            }
        }
        for(i=0;i<_constanttable.size();i++){
            if(_constanttable[i].name==var){
                return 1;
            }
        }
        int flag=0;
        int x;
        for(x=_index-1;x>=_indextable[_program_num]&&flag==0;x--){
            if(_signtable[x].name==var){
                flag=1;
                break;
            }
        }
        if(flag==1){
            return 1;
        }
        return -1;
    }

    int symboltable::isfunctionvariabledeclared(const std::string& var){
        int flag=0;
        int x;
        for(x=_index-1;x>=_indextable[_program_num]&&flag==0;x--){
            if(_signtable[x].name==var){
                flag=1;
                break;
            }
        }
        if(flag==1){
            return 1;
        }
        return -1;
    }

    void symboltable::addvariable(struct sign _sign){
        _sign.address=_index-_indextable[_program_num];
        _index++;
        _signtable.push_back(_sign);
        return;

    }

    void symboltable::addfunction(struct sign _sign){
        int x=isvariabledeclared(_sign.name);
        if(x!=-1){
            return;
        }
        else {
            _sign.type=2;
            _constanttable.push_back(_sign);
            _functiontable.push_back(_sign);
        }
    }

    void symboltable::editfuncitonparamNum(const std::string& funcname,int num){
        unsigned long long int i;
        for(i=0;i<_constanttable.size();i++){
            if(_constanttable[i].name==funcname){
                _constanttable[i].num_parameters=num;
                break;
            }
        }
         for(i=0;i<_functiontable.size();i++){
            if(_functiontable[i].name==funcname){
                _functiontable[i].num_parameters=num;
                break;
            }
        }
        return ;
    }

    std::pair<int32_t,int32_t> symboltable::getvariable(const std::string& name){
        int i;
        if(_program_num==0){
            for(i=_index-1;i>=0;i--){
                if(_signtable[i].name==name){
                    if(_signtable[i]._initialization==0){
                        return std::make_pair(-2,-2);//没有初始化；
                    }
                    else return std::make_pair(0,_signtable[i].address);
                }
            }
            return std::make_pair(-1,-1);
        }
        else{
            for(i=_index-1;i>=_indextable[_program_num];i--){
                if(_signtable[i].name==name){
                    if(_signtable[i]._initialization==0){
                        return std::make_pair(-2,-2);//没有初始化；
                    }
                    return std::make_pair(0,_signtable[i].address);
                }
            }
            for(i=0;i<_indextable[1];i++){
                if(_signtable[i].name==name){
                    if(_signtable[i]._initialization==0){
                        return std::make_pair(-2,-2);//没有初始化；
                    }
                    return std::make_pair(1,_signtable[i].address);
                }
            }
            return std::make_pair(-1,-1);
        }
    }

    std::pair<std::optional<sign>,int> symboltable::getvariable2(std::string& name){
        int i;
        struct sign _sign;
        for(i=_index-1;i>=_indextable[_program_num];i--){
            if(_signtable[i].name==name){
                _sign=_signtable[i];
                return std::make_pair(_sign,0);
            }
        }
        for(i=0;i<_indextable[1];i++){
            if(_signtable[i].name==name){
                _sign=_signtable[i];
                return std::make_pair(_sign,1);
            }
        }
        return std::make_pair(_sign,-1);
    }

    void symboltable::initialize(std::string& name){
        int i;
        for(i=_index-1;i>=_indextable[_program_num];i--){
            if(_signtable[i].name==name){
                _signtable[i]._initialization=1;
                return ;
            }
        }
        for(i=0;i<_indextable[1];i++){
            if(_signtable[i].name==name){
                 _signtable[i]._initialization=1;
                return ;
            }
        }
        return ;
    }

    std::pair<std::optional<sign>,int> symboltable::getfunction(std::string& name){
        unsigned long long int i;
        struct sign _sign;
        for(i=0;i<_constanttable.size();i++){
            if(_constanttable[i].name==name){
                _sign=_constanttable[i];
                return std::make_pair(_sign,i);
            }
        }
        return std::make_pair(_sign,-1);
    }

    std::vector<sign> symboltable::getsigntable(){
        return _signtable;
    }

    std::vector<sign> symboltable::getconstanttable(){
        return _constanttable;
    }

    std::vector<sign> symboltable::getfunctiontable(){
        return _functiontable;
    }

    std::vector<int> symboltable::getindextable(){
        return _indextable;
    }

    int symboltable::checkmain(){
        unsigned long long int i;
        for(i=0;i<_constanttable.size();i++){
            if(_constanttable[i].name=="main"){
                return 1;
            }
        }
        return 0;
    }
}