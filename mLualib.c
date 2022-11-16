/* Replace "dll.h" with the name of your header */

#include "mLualib.h"
#include <iostream>
#include "log.h"

using namespace std;
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

//待Lua调用的C注册函数
static int luaLog(lua_State *L)
{
    //检查栈中的参数是否合法，1表示Lua调用时的第一个参数(从左到右)，依此类推。
    //如果Lua代码在调用时传递的参数不为number，该函数将报错并终止程序的执行。
    const char *a = luaL_checkstring(L, 1);
    //将函数的结果压入栈中。如果有多个返回值，可以在这里多次压入栈中。
    // lua_pushstring(L,(const char *)a);
    LOGC("LOG_DEBUG", "%s", (const char *)a);
    //返回值用于提示该C函数的返回值数量，即压入栈中的返回值数量。
    return 1;
}

lua_State *g_L = NULL;

int initlua(CALCINFO *pData)
{
    g_L = luaL_newstate(); //创建Lua栈

    lua_checkstack(g_L, 60); //修改Lua栈大小为60，防止在C和Lua之间传递大数据时，崩溃

    luaL_openlibs(g_L); //运行Lua虚拟机

    // 注册C函数
    lua_register(g_L, "LOG", luaLog);

    int ret;
    double number = 0.0;

    // 设置时间数组
    lua_newtable(g_L);
    for (int i = 0; i < pData->m_nNumData; i++)
    {
        // c->stack，创建对应的key-val
        lua_pushnumber(g_L, pData->m_pData[i].m_time); //设置值
        lua_rawseti(g_L, -2, i + 1);                   //对应key-val
    }
    // stack->lua，将数组赋值到lua中，并弹出数组
    lua_setglobal(g_L, "timeArr");
    // 设置时间数组 end

    // 设置开盘数组
    lua_newtable(g_L);
    for (int i = 0; i < pData->m_nNumData; i++)
    {
        // c->stack，创建对应的key-val
        lua_pushnumber(g_L, pData->m_pData[i].m_fOpen); //设置值
        lua_rawseti(g_L, -2, i + 1);                    //对应key-val
    }
    // stack->lua，将数组赋值到lua中，并弹出数组
    lua_setglobal(g_L, "openArr");
    // 设置开盘数组 end

    // 设置最高价数组
    lua_newtable(g_L);
    for (int i = 0; i < pData->m_nNumData; i++)
    {
        // c->stack，创建对应的key-val
        lua_pushnumber(g_L, pData->m_pData[i].m_fHigh); //设置值
        lua_rawseti(g_L, -2, i + 1);                    //对应key-val
    }
    // stack->lua，将数组赋值到lua中，并弹出数组
    lua_setglobal(g_L, "highArr");
    // 设置最高价数组 end

    // 设置最低价数组
    lua_newtable(g_L);
    for (int i = 0; i < pData->m_nNumData; i++)
    {
        // c->stack，创建对应的key-val
        lua_pushnumber(g_L, pData->m_pData[i].m_fLow); //设置值
        lua_rawseti(g_L, -2, i + 1);                   //对应key-val
    }
    // stack->lua，将数组赋值到lua中，并弹出数组
    lua_setglobal(g_L, "lowArr");
    // 设置最低价数组 end

    // 设置收盘价数组
    lua_newtable(g_L);
    for (int i = 0; i < pData->m_nNumData; i++)
    {
        // c->stack，创建对应的key-val
        lua_pushnumber(g_L, pData->m_pData[i].m_fClose); //设置值
        lua_rawseti(g_L, -2, i + 1);                     //对应key-val
    }
    // stack->lua，将数组赋值到lua中，并弹出数组
    lua_setglobal(g_L, "closeArr");
    // 设置收盘价数组 end

    // 设置成交量数组
    lua_newtable(g_L);
    for (int i = 0; i < pData->m_nNumData; i++)
    {
        // c->stack，创建对应的key-val
        lua_pushnumber(g_L, pData->m_pData[i].m_fVolume); //设置值
        lua_rawseti(g_L, -2, i + 1);                      //对应key-val
    }
    // stack->lua，将数组赋值到lua中，并弹出数组
    lua_setglobal(g_L, "volArr");
    // 设置成交量数组 end

    // 设置成交额数组
    lua_newtable(g_L);
    for (int i = 0; i < pData->m_nNumData; i++)
    {
        // c->stack，创建对应的key-val
        lua_pushnumber(g_L, pData->m_pData[i].m_fAmount); //设置值
        lua_rawseti(g_L, -2, i + 1);                      //对应key-val
    }
    // stack->lua，将数组赋值到lua中，并弹出数组
    lua_setglobal(g_L, "amountArr");
    // 设置成交额数组 end
    
    // string s1 = "./Lua/LuaScript";
	// string s2 = "World! ";
    
    // 文件名
    float f = *pData->m_pfParam1;
    // LOGC("LOG_DEBUG", "File Name %d", (int)f);

    char *filestart = "./Lua/LuaScript_";
    int fileSeq = (int)f;
    char *fileEnd = ".lua";

    char *luaScriptPath = new char[strlen(filestart) + sizeof(fileSeq) + strlen(fileEnd)];
    sprintf(luaScriptPath, "%s%d%s", filestart, fileSeq, fileEnd);


    LOGC("LOG_DEBUG", "luaScriptPath  %s", luaScriptPath);

    if (ret = luaL_loadfile(g_L, luaScriptPath)) //装载lua脚本
    {
        cout << "load test.lua fail." << ret << endl;
        return -1;
    }
    else
    {
        cout << "load test.lua ok" << endl;
        if (ret = lua_pcall(g_L, 0, 0, 0)) //运行lua脚本
        {
            cout << "run test.lua fail..." << lua_tostring(g_L, -1) << endl;

            return -1;
        }
        else
        {
            // 从lua中取出全局的值
            lua_getglobal(g_L, "pResultBufStartPos");
            int pResultBufStartPos = lua_tonumber(g_L, -1);
            lua_pop(g_L, 1);
            LOGC("LOG_DEBUG", "Lua Return Value %d", pResultBufStartPos);

            // cout<<"run test.lua ok"<<endl;
            LOGC("LOG_DEBUG", "run test.lua ok");

            // 取出一个数组

            // lua->stack，得到全局数组，位置-1
            lua_getglobal(g_L, "pResultBuf");

            int index = lua_gettop(g_L); //获取Lua栈顶的内容的索引值

            lua_pushnil(g_L); // nil 入栈作为初始 key

            if (lua_isnil(g_L, index)) //栈顶内容为空
            {
                return -1;
            }

            int j = 0;

            while (lua_next(g_L, index)) //循环遍历栈中内容
            {
                // cout<<lua_tonumber(g_L, -1)<<" ";//取出栈顶内容
                number = lua_tonumber(g_L, -1);
                LOGC("LOG_DEBUG", "global_c_read_array  Value: %f", number);
                pData->m_pResultBuf[j] = number; //平均
                lua_pop(g_L, 1); //弹出栈顶内容，然后下一个数据处在栈顶
                j++;
            }

            return pResultBufStartPos;
        }
    }
}

void call_iface(CALCINFO *pData)
{
    LOGC("LOG_DEBUG", "call_iface write lua param");

    // c->stack，创建一个新的table放到-1的位置
    lua_newtable(g_L);

    int array[10] = {11, 22, 33, 44, 55, 66, 77, 88, 99, 100};

    for (int n = 0; n < 10; n++)
    {
        lua_pushinteger(g_L, array[n]); //将数组的数据入栈
    }

    // stack->lua，将数组赋值到lua中，并弹出数组
    lua_setglobal(g_L, "global_c_write_array");

    LOGC("LOG_DEBUG", "success ");
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        break;
    }
    case DLL_PROCESS_DETACH:
    {
        break;
    }
    case DLL_THREAD_ATTACH:
    {
        break;
    }
    case DLL_THREAD_DETACH:
    {
        break;
    }
    }

    /* Return TRUE on success, FALSE on failure */
    return TRUE;
}
__declspec(dllexport) int WINAPI2 MYMACLOSE(CALCINFO *pData)
{

    LOGC("LOG_DEBUG", "a=%d", 10);
    //    return 0;
    // if () //装载test.lua脚本
    // {

    //     // call_iface(pData); //调用test.lua脚本中提供的函数
    //     // LOG("run lua success %d ",1);
    //     // DWORD dwPid = ::GetCurrentProcessId();
    //     // DP0("[LYSM] Lying Simon tells the lie that he loves you so much.");
    // }
    // else
    // {
    //     LOGC("LOG_DEBUG", "init Lua Failed");
    // }

    // float f, fTotal;
    // int nPeriod, i, j;
    // if (pData->m_pfParam1 &&      //参数1有效
    //     pData->m_nParam1Start < 0 //参数1为常数
    //                               // && pData->m_pfParam2==NULL          //仅有一个参数
    // )
    // {
    //     f = *pData->m_pfParam1;
    //     nPeriod = (int)f; //参数1

    //     for (i = nPeriod - 1; i < pData->m_nNumData; i++) //计算nPeriod周期的均线,数据从nPeriod-1开始有效
    //     {
    //         fTotal = 0.0f;
    //         for (j = 0; j < nPeriod; j++) //累加
    //             fTotal += pData->m_pData[i - j].m_fClose;
    //         pData->m_pResultBuf[i] = fTotal / nPeriod; //平均
    //     }
    //     return nPeriod - 1;
    // }
    // return -1;
    int result = initlua(pData);
    return result;
}