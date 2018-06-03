#ifndef __j1VIDEO_H__
#define __j1VIDEO_H__

#include "j1Module.h"

#include <dshow.h>
#include <Vfw.h>                          // Header File For Video For Windows
#pragma comment( lib, "vfw32.lib" )                 // Search For VFW32.lib While Linking


class j1Video : public j1Module
{
public:

	j1Video();

	// Destructor
	virtual ~j1Video();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	void Initialize(char* file_path);
	void OpenAVI(LPCSTR path);
	bool GrabAVIFrame();

	void CloseAVI();

public:
	bool				isVideoFinished = false;

private:

	int			frame = 0;							// Frame Counter

	AVISTREAMINFO       psi;                        // Pointer To A Structure Containing Stream Info
	PAVISTREAM			pavi;                       // Handle To An Open Stream
	PGETFRAME			pgf;                        // Pointer To A GetFrame Object

	long				lastFrame;                  // Last Frame Of The Stream
	int					width;                      // Video Width
	int					height;                     // Video Height
	char*				pdata;					    // Pointer To Texture Data
	int					mpf;                        // Will Hold Rough Milliseconds Per Frame

	int i = 1;
};

#endif // __j1Video_H__
