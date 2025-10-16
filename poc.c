#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

static int exec(lua_State *L) {
    const char *command = luaL_checkstring(L, 1);
    
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }
    
    if (pid == 0) {
        // Child process
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);
        
        // Parse command into arguments
        char *cmd_copy = strdup(command);
        char *args[128];
        int argc = 0;
        char *token = strtok(cmd_copy, " ");
        
        while (token != NULL && argc < 127) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;
        
        // Execute the command
        execvp(args[0], args);
        
        // If execvp returns, an error occurred
        _exit(EXIT_FAILURE);
    }
    
    // Parent process
    close(pipefd[1]);
    
    // Read output from child
    luaL_Buffer buffer;
    luaL_buffinit(L, &buffer);
    char buf[512];
    ssize_t bytes_read;
    
    while ((bytes_read = read(pipefd[0], buf, sizeof(buf))) > 0) {
        luaL_addlstring(&buffer, buf, bytes_read);
    }
    close(pipefd[0]);
    
    // Wait for child process
    int status;
    waitpid(pid, &status, 0);
    
    // Push results to Lua
    luaL_pushresult(&buffer);
    
    if (WIFEXITED(status)) {
        lua_pushinteger(L, WEXITSTATUS(status));
    } else {
        lua_pushnil(L);
    }
    
    return 2; // Return output and exit code
}
