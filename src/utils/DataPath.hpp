#pragma once

#include "SDL.h"
#include <iostream>
#include <string>

class DataPath{
public:
    //! A getDataPath method, taking 0-1 argument.
    /*! Helper Function to get the datafolder.
     * Returns the data folder path and is windows safe, it can be parsed a string which is simply appended
     * \param  file a string pointer. String which will be appended (default is "").
     * \return string. The path to data folder and file param
     */
    static std::string get(const std::string &file = ""){
        //We need to choose the path separator properly based on which
        //platform we're running on, since Windows uses a different
        //separator than most systems
#ifdef _WIN32
        const char PATH_SEP = '\\';
#else
        const char PATH_SEP = '/';
#endif
        //This will hold the base data path:
        //We give it static lifetime so that we'll only need to call
        //SDL_GetBasePath once to get the executable path
        static std::string baseRes;
        if (baseRes.empty()){
            //SDL_GetBasePath will return NULL if something went wrong in retrieving the path
            char *basePath = SDL_GetBasePath();
            if (basePath){
                baseRes = basePath;
                SDL_free(basePath);
            }
            else {
                std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
                return "";
            }

#if DEBUG
            baseRes = baseRes + ".." + PATH_SEP + "data" + PATH_SEP;
#else
            baseRes = baseRes + "data" + PATH_SEP;
#endif
        }

        //If we want a specific file path in the data dir or just the data dir
        return file.empty() ? baseRes: baseRes + file;
    }

};
