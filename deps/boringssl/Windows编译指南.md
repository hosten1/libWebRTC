# 先检查环境

在当前 VS Developer Prompt 里执行：

```bash
where link
where ld
where ninja
```
正常应该类似：

`D:\soft\vs2022\VC\Tools\MSVC\...\link.exe`

如果出现：
`C:\msys64\mingw64\bin\ld.exe`

说明 PATH 被污染了。

# 修正
```bat
where link
where ld
where ninja
```
然后验证：
```bat
where ld
```
正常应该变成：`INFO: Could not find files for the given pattern(s).`;或者根本找不到 ld

建议删除build目录后，再检查下
```bat
where cl
where link
where ld
```
理想结果：

cl -> VS目录
link -> VS目录
INFO: Could not find files for the given pattern(s).

如果还有：

C:\msys64\mingw64\bin\ld.exe

说明系统环境变量里还有 MSYS2。

检查环境变量

执行：
```bat
set | findstr /I "LD="
set | findstr /I "CC="
set | findstr /I "CXX="
```
正常应该没有输出。

如果看到：
```bat
LD=C:\msys64\mingw64\bin\ld.exe
```
执行：
```bat
    set LD=
    set CC=
    set CXX=
```
强制指定 MSVC

然后重新配置：

```bash
cmake -G Ninja ^
  -B build ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_C_COMPILER=cl ^
  -DCMAKE_CXX_COMPILER=cl ^
  -DCMAKE_LINKER=link
```