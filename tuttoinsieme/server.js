{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "C++ Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceRoot}/server",
            "args": [
                "hello", "world", "!"
            ],
            "stopAtEntry": true,
            "cwd": "${workspaceRoot}",
            "linux": {
                "MIMode": "gdb",
                "miDebuggerPath": "/usr/bin/gdb"
            },
            "osx": {
                "MIMode": "lldb"
            },
            "windows": {
                "MIMode": "gdb",
                "miDebuggerPath": "C:\\MinGw\\bin\\gdb.exe"
            },
            "externalConsole": false
        }
    ]
}
