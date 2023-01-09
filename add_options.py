# add_warning.py
# Add different warning options for different compilers for native test.
#
import subprocess
Import("env")
#print(env.Dump())

gcc_options =  ['-Wreturn-local-addr', '-Werror=return-local-addr' ]
clang_options = ['-Wreturn-stack-address', '-Werror=return-stack-address' ]

# native
if 'PIOFRAMEWORK' not in env: 
    result = subprocess.getoutput('gcc --version | grep clang')
    if len(result) == 0:
        env.Append(BUILD_FLAGS = gcc_options)
    else:
        env.Append(BUILD_FLAGS= clang_options)
# embedded
else:
    env.Append(BUILD_FLAGS = gcc_options)
        
