/****************************************************************************
  
  GLUI User Interface Toolkit
  ---------------------------

     glui_filebrowser.cpp - GLUI_FileBrowser control class


          --------------------------------------------------

  Copyright (c) 1998 Paul Rademacher

  This program is freely distributable without licensing fees and is
  provided without guarantee or warrantee expressed or implied. This
  program is -not- in the public domain.

*****************************************************************************/

#include "glui.h"
#include "stdinc.h"
#include <sys/types.h>

#ifdef __GNUC__
#include <dirent.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <sys/stat.h>


/****************************** GLUI_FileBrowser::draw() **********/

void GLUI_FileBrowser::dir_list_callback(void *glui_object, int id) {
  GLUI_FileBrowser* me = (GLUI_FileBrowser*) glui_object;
  int this_item;
  char *selected;
  int i;
  if (me == NULL)
    return;
  if (me->list) {
    this_item = (me->list)->get_current_item();
    if (this_item > 0) { /* file or directory selected */
      selected = ((me->list)->get_item_ptr( this_item ))->text;
      if (selected[0] == '/' || selected[0] == '\\') {
        if (me->allow_change_dir) {
#ifdef __GNUC__
	      chdir(selected+1);
#endif
#ifdef _WIN32
			SetCurrentDirectory(selected+1);
#endif
       	me->fbreaddir(".");
        }
      } else {
	me->file = selected;
	me->execute_callback();
      }
    }
  }
}


void GLUI_FileBrowser::fbreaddir(char *d) {
  GLUI_String item;
  int i = 0;
	
	if (!d)
    return;

#ifdef _WIN32

  WIN32_FIND_DATA FN;
  HANDLE hFind;
  //char search_arg[MAX_PATH], new_file_path[MAX_PATH];
  //sprintf(search_arg, "%s\\*.*", path_name);
  
  hFind = FindFirstFile("*.*", &FN);
  if (list) {
    list->delete_all();
    if (hFind != INVALID_HANDLE_VALUE) {
      do {
        int len = strlen(FN.cFileName);
        if (FN.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
          item[0] = '\\';
          strncpy(item+sizeof(char), FN.cFileName, len+1);
          item[len+1] = '\0';
        } else {
          strncpy(item, FN.cFileName, len);
          item[len] = '\0';
        }
        list->add_item(i,item);
        i++;
      } while (FindNextFile(hFind, &FN) != 0);
      
      if (GetLastError() == ERROR_NO_MORE_FILES)
        FindClose(&FN);
      else
        perror("fbreaddir");
    }
  }

#elif defined(__GNUC__)

  DIR *dir;
  struct dirent *dirp;
  struct stat dr;

  if (list) {
    list->delete_all();
    if ((dir = opendir(d)) == NULL)
      perror("fbreaddir:");
    else {
      while ((dirp = readdir(dir)) != NULL) { /* open directory     */
	if (!lstat(dirp->d_name,&dr)       /* dir can be stated  */
	    && S_ISDIR(dr.st_mode)) {         /* dir is directory   */
	    item[0] = '/';
	    strncpy(item+sizeof(char), dirp->d_name, strlen(dirp->d_name)+1);
            item[strlen(dirp->d_name)+1] = '\0';
	} else {
	  strncpy(item, dirp->d_name, strlen(dirp->d_name));
          item[strlen(dirp->d_name)] = '\0';
	}
	list->add_item(i,item);
	i++;
      }
      closedir(dir);
    }

  }
#endif
}

void ProcessFiles(const char *path_name)
{	

}


void GLUI_FileBrowser::set_w(int w) { if (list) list->set_w(w); }
void GLUI_FileBrowser::set_h(int h) { if (list) list->set_h(h); }