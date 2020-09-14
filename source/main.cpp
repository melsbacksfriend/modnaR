// Include the most common headers from the C standard library
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <stdio.h>
#include <dirent.h>
#include <cstring>
#include <algorithm>
#include <sys/stat.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

bool CheckIsDir(std::string Path)
{
	struct stat statbuf;
	stat(Path.c_str(), &statbuf);
	return S_ISDIR(statbuf.st_mode);
}

//Stolen from https://stackoverflow.com/a/12774387
bool CheckFileExists(std::string Path)
{
	struct stat buffer;   
	return (stat (Path.c_str(), &buffer) == 0); 
}

void RecursiveFileCopy(std::string SourcePath, std::string DestPath, std::string FileName)
{
	//If dir
	if(CheckIsDir(SourcePath))
	{
		//Don't copy a dir in to it's self
		if(DestPath.find(SourcePath) == 0) return;
		//Make the new dir
		std::string NewDirPath = DestPath + "/" + FileName;
		mkdir(NewDirPath.c_str(), 0);
		//For each file in the source path call this function
		DIR* dir;
		struct dirent* ent;
		dir = opendir(SourcePath.c_str());
		while ((ent = readdir(dir)))
		{
			//Get path of file we want to copy
			std::string PathToCopy = SourcePath + "/" + ent->d_name;
			RecursiveFileCopy(PathToCopy.c_str(), NewDirPath.c_str(), ent->d_name);
		}
		closedir(dir);
	}
	//If file just copy it
	else if(CheckFileExists(SourcePath.c_str()))
	{
		//get the full path of the destination file
		std::string PathToCopyTo = DestPath + "/" + FileName;
		//Get files as streams
		std::ifstream SourceFile (SourcePath, std::ifstream::binary);
		std::ofstream DestFile (PathToCopyTo, std::ofstream::binary);
		//Get file size
		SourceFile.seekg (0,SourceFile.end);
		long size = SourceFile.tellg();
		SourceFile.seekg (0);
		//Create a 0.25 gb buffer
		int Chunksize = 1024 * 1024 * 256;
		//If the file is smaller than the buffer change the chunk size
		if(size < Chunksize) Chunksize = size;
		char* Buffer = new char[Chunksize];
		//Calculate how much of the file will be left over when split in to chunks
		int LeftOvers = size % Chunksize;
		while(SourceFile.tellg() != size - LeftOvers)
		{
			//Copy the source in to the buffer
			SourceFile.read(Buffer,Chunksize);
			//Writet he buffer in to the destination file
			DestFile.write(Buffer,Chunksize);
		}
		//Copy the left over bytes that didn't fit evenly in to a buffer
		SourceFile.read(Buffer,LeftOvers);
		DestFile.write(Buffer,LeftOvers);
		//Clean up
		delete[] Buffer;
		DestFile.close();
		SourceFile.close();
	}
}

// Main program entrypoint
int main(int argc, char* argv[])
{
    // This example uses a text console, as a simple way to output text to the screen.
    // If you want to write a software-rendered graphics application,
    //   take a look at the graphics/simplegfx example, which uses the libnx Framebuffer API instead.
    // If on the other hand you want to write an OpenGL based application,
    //   take a look at the graphics/opengl set of examples, which uses EGL instead.
    consoleInit(NULL);

    // Other initialization goes here. As a demonstration, we print hello world.
    std::cout << "Warning!  This is for Atmosphère versions\nwith a \"contents\" folder!\nAnd also, this software was written by\na person with ANGER ISSUES!\nI'm not responsible for any damage!\n\nPress A to continue.\n";

    // Main loop
    while (appletMainLoop())
    {
        // Scan all the inputs. This should be done once for each frame
        hidScanInput();

        // hidKeysDown returns information about which buttons have been
        // just pressed in this frame compared to the previous one
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS)
            break; // break in order to return to hbmenu

        if (kDown & KEY_A)
		{
			srand(time(0));
			std::ifstream modcount("sdmc:/modcount.txt");
			Result rc=0;
			SwkbdConfig kbd;
			char tmpoutstr[50] = {0};
			rc = swkbdCreate(&kbd, 0);
			swkbdConfigMakePresetUserName(&kbd);
			swkbdConfigSetHeaderText(&kbd, "Name of game folder (see readme)");
			rc = swkbdShow(&kbd, tmpoutstr, sizeof(tmpoutstr));
			std::string gamename = tmpoutstr;
			std::string modcountstr;
			std::string tid;
			if (!modcount.is_open())
			{
				std::cout << "You didn't put the number of mods\nin sdmc:/modcount.txt\n";
			}
			if (modcount.is_open())
			{
				std::getline(modcount, modcountstr, '\n');
				int modcountint = std::stoi(modcountstr);
				std::string currentindex = std::to_string(1 + (rand() % (modcountint)));
				for (auto& p: std::filesystem::directory_iterator("sdmc:/modnaR/" + gamename + "/" + currentindex))
				{
					tid = p.path().filename().string();
				}
				for (auto& p: std::filesystem::directory_iterator("sdmc:/atmosphere/contents"))
				{
					if (p.path().filename().string() == tid)
					{
						std::filesystem::remove_all(p.path());
					}
				}
				RecursiveFileCopy("sdmc:/modnaR/" + gamename + "/" + currentindex + "/" + tid, "sdmc:/atmosphere/contents", tid);
				std::cout << "DONE!\n";
			}
		}

        // Update the console, sending a new frame to the display
        consoleUpdate(NULL);
    }

    // Deinitialize and clean up resources used by the console (important!)
    consoleExit(NULL);
    return 0;
}
