#pragma once
// This file contains Doxygen Qt style comments

/*! \file CEnum.cpp - a single file that contains CEnum class*/

//#define UNICODE
//#define _UNICODE
//#define MFC
#include <string>
#include <list>
#include "tchar.h"

#ifdef MFC
    #include "afxcoll.h"
#else
    #include <windows.h>
#endif
#include <atlstr.h>
#include <vector>

using namespace std;

typedef basic_string<TCHAR> _stl_string;    //!< basic_string<TCHAR> will compile into std::string or 
                                            //!< std::wstring, based on if _UNICODE was defined

/*!
\brief      CEnum - File Enumeration and File Globbing Class

CEnum is a fast and simple class that lets you enumerate (i.e. make a list of) all files in a directory.
It supports: 
         - the use of wild card characters * and ? (i.e. its a file globbing class too) 
         - separate enumeration of files and subdirectories 
         - separate include list, exclude list and ignore case option for both files and directories 
         - recursive search (i.e. it can enumerate contest of subdirectories too) 
         - enumeration either using file's full path or just file name 
         - written in STL, but it optionally supports MFC collection classes 
         - UNICODE aware (in MS world UNICODE stands for UTF-16 little-endian) 
         - works as a wrapper around ::FindFirstFile and ::FindNextFile 

\author     Loreia (loreia (at) net.hr)
\version    1.1
\date       published on www.codeproject.com on 11/2008
\par    License
There is no limitation on use of this class. 
You may freely use, redistribute and modify this code (as whole or just parts of it) with or without author's 
premission in any sort of project regardless of license issues, in both commercial and open source projects.
\par Errata
-# Fatal error C1010: unexpected end of file while looking for precompiled header.\n
     Did you forget to add '\#include "stdafx.h"' to your source?\n
- Solution:\n
        -# In the Solution Explorer pane of the project, right-click the project name, and then click Properties.\n
        -# In the left pane, click the C/C++ folder. \n
        -# Click the Precompiled Headers node. \n
        -# In the right pane, click Create/Use Precompiled Header, and then click Not Using Precompiled Headers. \n
        
*/
class CEnum
{
public:
/** @name
Public member variables are used to set desired options. 
Default values are empty strings _T("") for all Pattern strings, and false for bool values.
In case of conflict, Exclude pattern has precedence over Include pattern.
Multiple search strings can be separated by a semicolon ';' character.
*/
//@{
    _stl_string sExcPatternDirs;            //!< Exclude pattern in directory search.
    _stl_string sExcPatternFiles;           //!< Exclude pattern in file search.
    _stl_string sIncPatternDirs;            //!< Include pattern in directory search.
    _stl_string sIncPatternFiles;           //!< Include pattern in file search.

    bool bRecursive;                        //!< Enumerate content of subdirectories too.
    bool bFullPath;                         //!< Enumeration using file's full path.
    bool bNoCaseDirs;                       //!< Ignore case when searching for directories (and only directories!).
    bool bNoCaseFiles;                      //!< Ignore case when searching for files (and only files!).
//@}
private:
/** @name
Notice the space in front of the right angle bracket in declaration of all six lists!
*/
//@{
    list<_stl_string > * plstDirs;          //!< This std::list holds a list of directories found in a search.
    list<_stl_string > * plstFiles;         //!< This std::list holds a list of files found in a search.


    list<_stl_string > plstExcPatternDirs;  //!< User supplied, holds Exclude pattern(s) for directory search.
    list<_stl_string > plstExcPatternFiles; //!< User supplied, holds Exclude pattern(s) for file search.
    list<_stl_string > plstIncPatternDirs;  //!< User supplied, holds Include pattern(s) for directory search.
    list<_stl_string > plstIncPatternFiles; //!< User supplied, holds Include pattern(s) for file search.
//@}

#ifdef MFC
/** @name MFC containers
This is used only in case user wants to convert std:: containers to their MFC counterparts and is, by default, inactive.
*/
//@{
    CStringArray * pCSArrayDirs;            //!< List of directories found in a search converted to MFC's CStringArray. 
    CStringArray * pCSArrayFiles;           //!< List of files found in a search converted to MFC's CStringArray. 
    CStringList  * pCSListDirs;             //!< List of directories found in a search converted to MFC's CStringList. 
    CStringList  * pCSListFiles;            //!< List of files found in a search converted to MFC's CStringList. 
//@}
#endif  

public:
//! Default constructor
/*! 
Default constructor initializes all members to the most common values.
If you wish, change defaults to your liking.
*/
    CEnum()
    {
        plstDirs            = new list<_stl_string >;
        plstFiles           = new list<_stl_string >;
        
        sExcPatternDirs     = _T("");
        sExcPatternFiles    = _T("");
        sIncPatternDirs     = _T("");
        sIncPatternFiles    = _T("");
        
        bRecursive          = false;
        bFullPath           = false;
        bNoCaseDirs         = false;
        bNoCaseFiles        = false;

#ifdef MFC
		pCSArrayDirs		= NULL; 
		pCSArrayFiles		= NULL;
		pCSListDirs			= NULL;
		pCSListFiles		= NULL;
#endif
    }

//! Parametrized constructor
/*! 
Parametrized constructor uses the same defaults for member vaiables 
as default constructor...
\sa EnumerateAll()
*/
    CEnum
        (
            _stl_string sPath,
            _stl_string sExcludePatternDirs     = _T(""),
            _stl_string sExcludePatternFiles    = _T(""),
            _stl_string sIncludePatternDirs     = _T(""),
            _stl_string sIncludePatternFiles    = _T(""),
            bool bRecursiveSearch               = false,
            bool bUseFullPath                   = false,
            bool bIgnoreCaseDirs                = false,
            bool bIgnoreCaseFiles               = false
        )
    {
        plstDirs          = new list<_stl_string >;
        plstFiles         = new list<_stl_string >;

        sExcPatternDirs   = sExcludePatternDirs;
        sExcPatternFiles  = sExcludePatternFiles;
        sIncPatternDirs   = sIncludePatternDirs;
        sIncPatternFiles  = sIncludePatternFiles;

        bRecursive        = bRecursiveSearch;
        bFullPath         = bUseFullPath;
        bNoCaseDirs       = bIgnoreCaseDirs;
        bNoCaseFiles      = bIgnoreCaseFiles;

#ifdef MFC
		pCSArrayDirs	  = NULL; 
		pCSArrayFiles	  = NULL;
		pCSListDirs		  = NULL;
		pCSListFiles	  = NULL;
#endif

        EnumerateAll(sPath);
    }

//! Destructor
/*! 
If you don't want to free memory allocated by CEnum
than comment out some (or all) lines of destructor.
*/
    ~CEnum()
    {
        if (plstDirs)	delete plstDirs;
        if (plstFiles)	delete plstFiles;
        plstDirs = NULL;
        plstFiles = NULL;
        
#ifdef MFC
        if (pCSArrayDirs)	delete pCSArrayDirs;
        if (pCSArrayFiles)	delete pCSArrayFiles;
        if (pCSListDirs)	delete pCSListDirs;
        if (pCSListFiles)	delete pCSListFiles;    
            
        pCSArrayDirs  = NULL;    
        pCSArrayFiles = NULL;    
        pCSListDirs   = NULL;        
        pCSListFiles  = NULL;    
#endif
    }

private:

//! Wildcard compare functionality. If desired, strings can be compared regardless of case.
/*!
\param      sPattern        Wildcard string, it can contain wild card characters '?' (match exactly one char) and '*' (match zero or more chars)
\param      sFileName    A string to compare to sPattern, (in this class it is always a file name)
\param      bNoCase      If true, case will be ignored when comparing strings
\return     The test result, true if strings match, false otherwise
\sa             CompareList(), EnumerateAll()
\note        '_tcsicmp' function requires to set locale by calling '_tsetlocale'
*/
    bool CompareStrings(LPCTSTR sPattern, LPCTSTR sFileName, bool bNoCase)
    {
        TCHAR temp1[2] = _T("");
        TCHAR temp2[2] = _T("");
        LPCTSTR pStar  = 0;
        LPCTSTR pName  = 0;
        
        while(*sFileName)
        {
            switch (*sPattern)
            {
            case '?':
                ++sFileName; ++sPattern;
                continue;
            case '*':
                if (!*++sPattern) return 1;
                pStar = sPattern;
                pName = sFileName + 1;
                continue;
            default:
                if(bNoCase) 
                {
                    // _tcsicmp works with strings not chars !!
                    *temp1 = *sFileName;
                    *temp2 = *sPattern;
                    if (!_tcsicmp(temp1, temp2))     // if equal
                    {
                        ++sFileName; 
                        ++sPattern; 
                        continue;
                    }
                }
                else if (*sFileName == *sPattern)    // bNoCase == false, 
                {                                    // compare chars directly
                    ++sFileName; 
                    ++sPattern; 
                    continue;
                }

                // chars are NOT equal, 
                // if there was no '*' thus far, strings don't match
                if(!pStar) return 0;

                // go back to last '*' character
                sPattern = pStar;
                sFileName = pName++;
                continue;
            }
        }
        // check is there anything left after last '*'
        while (*sPattern == '*') ++sPattern;
        return (!*sPattern);
    }

//! Compare file name with one or more search patterns that user supplied.
/*!
\param      plsPattern      STL list of tokenized search patterns.
\param      sFileName       A string to compare to search pattern(s), (in this class it is always a file name)
\param      bNoCase         If true, case will be ignored when comparing strings
\return     The test results, true if at least one search string matches file name, false otherwise
\sa Tokenize(), CompareStrings()
*/
    bool CompareList(list<_stl_string > & plsPattern, _stl_string & sFileName, bool bNoCase)
    {
        list<_stl_string >::iterator iter = plsPattern.begin();

        for(; iter != plsPattern.end(); ++iter)
        {
            if(CompareStrings(iter->c_str(), sFileName.c_str(), bNoCase)) return true;
        }
        
        return false;
    }

//! Start enumerating root directory
/*!
This function does the actual enumeration. 
It scans for files and dirs and enumerates them if they match search criteria. 
If needed, function can call itself to enumerate subdirectories.
\param  sPath   Root directory to start your search from.
\sa     EnumerateFile(), EnumerateDir()

\warning    After enumerating files in each directory, STL list's sort() method is called to sort list in alphabetic order. 
This may cause file names to be sorted in different order than in windows explorer.
If you wish you can comment out a call to sort() method, and provide your own sorting functionallity.
*/    
    void Enumerate(_stl_string & sPath)
    {
        WIN32_FIND_DATA         FindFileData;
        HANDLE                  hFind           = INVALID_HANDLE_VALUE;
        _stl_string             sFileName       = _T("");
        _stl_string             sSubDir         = _T("");
        
        // These are temporary lists that hold files and dirs found in this directory only.
        // I could skip this step, and enumerate directly to global list(s), 
        // but I wanted to sort() per directory, not per global list.
        // This gives much nicer format of the global list
        list<_stl_string >      pListDirs;
        list<_stl_string >      pListFiles;

        sPath += _T("\\");

        // find the first file in the directory.
        hFind = ::FindFirstFile((sPath + _T("*")).c_str(), &FindFileData);

        if (hFind != INVALID_HANDLE_VALUE)
        {
            sFileName = FindFileData.cFileName;
            if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                EnumerateFile(sPath, sFileName, pListFiles);
            }
            else if (sFileName.compare(_T(".")) && sFileName.compare(_T("..")))
            {
                EnumerateDir(sPath, sFileName, pListDirs);                
            }

            // find all the other files in this directory.
            while (::FindNextFile(hFind, &FindFileData) != 0) 
            {
                sFileName = FindFileData.cFileName;
                if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    EnumerateFile(sPath, sFileName, pListFiles);
                }
                else if (sFileName.compare(_T(".")) && sFileName.compare(_T("..")))
                {
                    EnumerateDir(sPath, sFileName, pListDirs);                                
                }
            }
        }

        // sort in alphabetic order
        pListDirs.sort();
        pListFiles.sort();

        // directory search is finished, copy what was found to global lists
        plstDirs->insert(plstDirs->end(), pListDirs.begin(), pListDirs.end());
        plstFiles->insert(plstFiles->end(), pListFiles.begin(), pListFiles.end());
        
        // list's destructor will do it anyway, but why not free some memory right here
        pListFiles.clear();

        // OK, lets check out those subdirectories
        if (bRecursive)
        {
            list<_stl_string >::iterator iter = pListDirs.begin();
            for (;iter != pListDirs.end(); ++iter)
            {
                if (bFullPath)
                {
                    Enumerate(*iter);
                }
                else
                {
                    sSubDir = sPath + *iter;
                    Enumerate(sSubDir);
                }
            }
        }

        FindClose(hFind);
    }

//! Enumerate a single file
/*!
\param      sPath               Path to directory where file resides.
\param      sFileName       Name of the file that is about to be enumerated.
\param      pList               A reference to STL list in which file will be enumerated
\sa     Enumerate(), EnumerateDir()
*/    
    void EnumerateFile(_stl_string & sPath, _stl_string & sFileName, list<_stl_string > & pList)
    {
        if // enumerate file if (both patterns are empty) or (if exclude isn't found and include pattern is found)
            (
                (sExcPatternFiles.empty() || !CompareList(plstExcPatternFiles, sFileName, bNoCaseFiles))
                 && 
                (sIncPatternFiles.empty() || CompareList(plstIncPatternFiles, sFileName, bNoCaseFiles))
            )
        {
            if(bFullPath) pList.push_back(sPath + sFileName);
            else pList.push_back(sFileName);
        }
    }

//! Enumerate a single directory
/*!
\param      sPath               Path to directory where newly found directory item resides.
\param      sDirName        Name of the directory that is about to be enumerated.
\param      pList               A reference to STL list in which item will be enumerated.
\sa     Enumerate(), EnumerateFile()
*/      
    void EnumerateDir(_stl_string & sPath, _stl_string & sDirName, list<_stl_string > & pList)
    {
        if // enumerate directory if (both patterns are empty) or (if exclude isn't found and include pattern is found)
            (
                (sExcPatternDirs.empty() || !CompareList(plstExcPatternDirs, sDirName, bNoCaseDirs))
                 && 
                (sIncPatternDirs.empty() || CompareList(plstIncPatternDirs, sDirName, bNoCaseDirs))
            )
        {
            if(bFullPath) pList.push_back(sPath + sDirName);
            else pList.push_back(sDirName);
        }
    }

//! Tokenize the search string, i.e. split it by tokenize character
/*!
\param          plsTokenized        STL list that holds tokenizes strings.
\param          sPattern                Pattern string that user supplied, it may or may not contain multiple search strings separated by tokenize character.
\sa     EnumerateAll()
\note Tokenize character is ';' (semicolon)
*/     
    void Tokenize(list<_stl_string > & plsTokenized, _stl_string & sPattern)
    {
        // search strings are tokenized by ';' (semicolon) character

        string::size_type position = 0;
        string::size_type length = 0;

        while (length != string::npos)
        {
            position = sPattern.find_first_not_of(';', length);
            length = sPattern.find_first_of(';', length + 1);

            plsTokenized.push_back(sPattern.substr(position, length - position));
        }
    }

public:

//! Start enumerating
/*!
This is one and only public method (because MFC stuff is not compiled by default), and it does all the work.
After setting public member variables, user only needs to call this method.
\param  sPath   Root directory to start your search from.
\sa     Enumerate(), Tokenize()
*/   
    void EnumerateAll(_stl_string sPath)
    {   
        // in case we are reusing the same object
        if(plstDirs) plstDirs->clear();
        if(plstFiles) plstFiles->clear();

        // basic error checking
        if (sPath.empty()) return;
        if (sPath[sPath.size() - 1] == '\\') sPath.erase(sPath.size() - 1);    // remove last backslash

        // sets the locale to the user-default ANSI code page obtained from the operating system
        // required by _tcsicmp function (see CompareStrings function)
        if(bNoCaseDirs || bNoCaseFiles)
        {
            if( _tsetlocale(LC_ALL, _T("")) == NULL) return;
        }

        // tokenize all search patterns
        if(!sExcPatternDirs.empty())    Tokenize(plstExcPatternDirs,  sExcPatternDirs);
        if(!sExcPatternFiles.empty())   Tokenize(plstExcPatternFiles, sExcPatternFiles);
        if(!sIncPatternDirs.empty())    Tokenize(plstIncPatternDirs,  sIncPatternDirs);
        if(!sIncPatternFiles.empty())   Tokenize(plstIncPatternFiles, sIncPatternFiles);

        Enumerate(sPath);    // fasten your seat belts, we are ready to go
    }

//! Get a pointer to STL list of directories found
/*!
After enumeration is done, call this method to obtain pointer to STL list of directories found.
This object is free'd in CEnum's destructor 
\sa     Enumerate()
*/
    list<_stl_string > * GetDirs() { return plstDirs; }

//! Get a pointer to STL list of files found
/*!
After enumeration is done, call this method to obtain pointer to STL list of files found.
This object is free'd in CEnum's destructor 
\sa     Enumerate()
*/
    list<_stl_string > * GetFiles() { return plstFiles; }

#ifdef MFC

/** @name MFC conversion functions
These functions are hidden behind preprocessor directive \#ifdef MFC, and are by default inactive (not compiled).
*/
//@{

//! Convert STL list of directories found into a MFC CStringArray
/*!
\return     A pointer to new MFC CStringArray containing the list of directories found.
\sa     GetDirsAsCStringList()
*/
    CStringArray * GetDirsAsCStringArray()
    {
        pCSArrayDirs = new CStringArray();
        list<_stl_string >::iterator iter = plstDirs->begin();
        for (;iter != plstDirs->end(); ++iter)
        {
            pCSArrayDirs->Add(iter->c_str());
        }
        return pCSArrayDirs;
    }

//! Convert STL list of files found into a MFC CStringArray
/*!
\return     A pointer to new MFC CStringArray containing the list of files found.
\sa     GetFilesAsCStringList()
*/    
    CStringArray * GetFilesAsCStringArray() 
    {
        pCSArrayFiles = new CStringArray();
        list<_stl_string >::iterator iter = plstFiles->begin();
        for (;iter != plstFiles->end(); ++iter)
        {
            pCSArrayFiles->Add(iter->c_str());
        }
        return pCSArrayFiles;
    }

//! Convert STL list of directories found into a MFC CStringList
/*!
\return     A pointer to new MFC CStringList containing the list of directories found.
\sa     GetDirsAsCStringArray()
*/  
    CStringList  * GetDirsAsCStringList()
    {
        pCSListDirs = new CStringList();
        list<_stl_string >::iterator iter = plstDirs->begin();
        for (;iter != plstDirs->end(); ++iter)
        {
            pCSListDirs->AddTail(iter->c_str());
        }
        return pCSListDirs;
    }

//! Convert STL list of files found into a MFC CStringList
/*!
\return     A pointer to new MFC CStringList containing the list of files found.
\sa     GetFilesAsCStringArray()
*/      
    CStringList  * GetFilesAsCStringList()
    {
        pCSListFiles = new CStringList();
        list<_stl_string >::iterator iter = plstFiles->begin();
        for (;iter != plstFiles->end(); ++iter)
        {
            pCSListFiles->AddTail(iter->c_str());
        }
        return pCSListFiles;
    }
//@}    





#endif

};
void zjlSearchFile(vector<CStringA>& vResult,CString szSearchPath,CString szFlag,bool bFullPath=1,bool bSubPath=1)
{
	CString strSearchPath=szSearchPath;
	CString strFlag=szFlag;
	CEnum enumerator;
	enumerator.bRecursive = bSubPath;          // search subdirectories too
	enumerator.bFullPath  = bFullPath;          // enumerate using file's full path
	enumerator.sIncPatternFiles  = strFlag;
	// start search
	enumerator.EnumerateAll(strSearchPath.GetBuffer());   // root directory to start your search from

	//list<string> * pAllFilesOnCDrive = enumerator.GetFiles();
	list<basic_string<TCHAR> > * plstFiles = enumerator.GetFiles();
	list<basic_string<TCHAR>>::iterator iter = plstFiles->begin();

	CStringA strTemp;
	for(;iter != plstFiles->end(); ++iter)
	{
		strTemp=iter->c_str();
		vResult.push_back(strTemp);
	}
}



/*

void zjlSearchFile(vector<CStringW>& vResult,CString strSearchPath,CString strFlag,bool bFullPath=1,bool bSubPath=1)
{
	CEnum enumerator;
	enumerator.bRecursive = bSubPath;          // search subdirectories too
	enumerator.bFullPath  = bFullPath;          // enumerate using file's full path
	enumerator.sIncPatternFiles  = strFlag.GetBuffer();
	// start search
	enumerator.EnumerateAll(strSearchPath.GetBuffer());   // root directory to start your search from

	//list<string> * pAllFilesOnCDrive = enumerator.GetFiles();
	list<basic_string<TCHAR> > * plstFiles = enumerator.GetFiles();
	list<basic_string<TCHAR>>::iterator iter = plstFiles->begin();

	CStringW strTemp;
	for(;iter != plstFiles->end(); ++iter)
	{
		strTemp=iter->c_str();
		vResult.push_back(strTemp);
	}
}

*/


/*! \mainpage A simple manual
Here you can find some tips how to use CEnum.\n

- \subpage intro1
- \subpage intro2
- \subpage basic
- \subpage advanced

\image html ./CEnum1.png

*/
//-----------------------------------------------------------
/*! \page intro1 What CEnum is?
CEnum is a file enumeration class.\n
It enumerates (i.e. makes a list of) files (and directories) based on file name. \n\n
It supports: 
         - the use of wild card characters * and ? (i.e. its a file globbing class too) 
         - separate enumeration of files and subdirectories 
         - separate include list, exclude list and ignore case option for both files and directories 
         - recursive search (i.e. it can enumerate contest of subdirectories too) 
         - enumeration either using file's full path or just file name 
         - written in STL, but it optionally supports MFC collection classes 
         - UNICODE aware (in MS world UNICODE stands for UTF-16 little-endian) 
         - works as a wrapper around ::FindFirstFile and ::FindNextFile 
         
Next section \ref intro2
*/
//-----------------------------------------------------------
/*! \page intro2 What CEnum isn't?
CEnum isn't a comprehensive file search tool.\n
It only searches for file name, it can't search for files based on:
        - file size
        - file date
        - file attributes
        - file content

Previous \ref intro1 Next \ref basic
*/
//-----------------------------------------------------------
/*! \page basic Basic Usage
This page is for users who's main objective can be summed up as: \n
<i>"Forget the details and let me enumerate all files in my working directory in less than five seconds of coding"</i>\n

\n<b>Example 1: Enumerate all files on C: drive</b>
<pre lang="c++">

CEnum enumerator;
enumerator.bRecursive = true;          // search subdirectories too
enumerator.bFullPath  = true;          // enumerate using file's full path

<span class="code-comment">//</span> start search
enumerator.EnumerateAll(_T("C:\\"));   // root directory to start your search from

list&lt;string&gt; * pAllFilesOnCDrive = enumerator.GetFiles();

<span class="code-comment">//</span> congrats, you have just created a list of all files on C: drive
<span class="code-comment">//</span> lets print it

list&lt;string&gt;:: iterator iter = pAllFilesOnCDrive-&gt;begin();
for (; iter != pAllFilesOnCDrive-&gt;end(); ++iter)
{
    cout &lt;&lt; iter-&gt;c_str();
}

<span class="code-comment">//</span> at this point just move on,
<span class="code-comment">//</span> CEnum will release memory allocated by pAllFilesOnCDrive for you
</pre>

\n
\n

<b>Example 2: enumerate all MP3's on C: drive</b>
<pre lang="c++">// first uncomment this line: //# define MFC, then recompile

CEnum enumerator;
enumerator.sIncPatternFiles  = _T("*.mp3");
enumerator.bNoCaseFiles      = true;       // enumerate both *.mp3 and *.MP3
enumerator.bRecursive        = true;
enumerator.bFullPath         = true;


<span class="code-comment">//</span> start search
enumerator.EnumerateAll(_T("C:"));         // unlike in the first example, this time
                                           <span class="code-comment">//</span> there is no backslash at the end
                                           <span class="code-comment">//</span> of path string

CStringArray * pCStringArray = enumerator.GetFilesAsCStringArray();
for(int i=0; i&lt;pCStringArray-&gt;GetSize(); ++i)
{
    cout &lt;&lt; (LPCTSTR) pCStringArray-&gt;GetAt[i];
}


<span class="code-comment">//</span> or if you preffer CStringList...
CStringList  * pCStringList  = enumerator.GetFilesAsCStringList();
POSITION pos = pCStringList-&gt;GetHeadPosition();
while (pos != NULL)
{
    cout &lt;&lt; (LPCTSTR) pCStringList-&gt;GetNext(pos);
}

<span class="code-comment">//</span> once again, memory management is taken care of by CEnum.
<span class="code-comment">//</span> at this point you can just forget that pCStringArray and pCStringList even existed
</pre>

\n
\note For your convenience, the path to root folder may or may not end with backslash!

This is already enough knowledge for your basic use of CEnum, for details read \ref advanced

*/
//-----------------------------------------------------------
/*! \page advanced Advanced Usage
This page is for advanced users.
Make sure you have first read \ref basic




<h3><a name="Instantiating4">Instantiating</a></h3>

<p>There are two constructors for greater flexibility when using <code>CEnum</code></p>

<pre lang="c++">// ... using default constructor

CEnum enumerator;

<span class="code-comment">//</span> set options
enumerator.sExcPatternDirs  = _T("Post Black album Metallica");
enumerator.sIncPatternDirs  = _T("Iron Maiden");
enumerator.sExcPatternFiles = _T("*.wma");
enumerator.sIncPatternFiles = _T("*.mp3;*.ogg");
enumerator.bRecursive       = true;    
enumerator.bFullPath        = true;
enumerator.bNoCaseDirs      = true;    
enumerator.bNoCaseFiles     = true;

enumerator.EnumerateAll(_T("D:\\Music"));

list&lt;string&gt; * pQualityMetal = enumerator.GetFiles();</pre>
\n
<p><b>... or the same thing with fewer lines: </b></p>

<pre lang="c++">// ... using parameterized constructor

CEnum enumerator(   
                    _T("D:\\Music"), 
                    _T("Post Black album Metallica"), 
                    _T("*.wma"),
                    _T("Iron Maiden"),
                    _T("*.mp3;*.ogg"),
                    true,
                    true,
                    true,
                    true
                 );

list&lt;string&gt; * pQualityMetal = enumerator.GetFiles();</pre>

\n

<p><b>... or the really simple way (if default values work for you):</b></p>

<pre lang="c++">// ... using parameterized constructor with default values

CEnum enumerator( _T("D:\\Music") );

list&lt;string&gt; * pQualityMetal = enumerator.GetFiles();</pre>

\n
<h3><a name="Classdestructor5">Class Destructor</a></h3>

<p>Let me quote the very first book I read about C++, Jesse Liberty's "Teach yourself C++ in 21 days": 
"If you are writing a function that needs to create memory and then pass it back to the calling function, 
consider changing your interface. Have the calling function allocate the memory and then pass it into your function by reference. 
This moves all memory management out of your program and back to the function that is prepared to delete it."</p>

<p>Why did I not follow this great advice? Well firstly, this was designed to be similar to C# class <code>Directory</code> (and in C# garbage collector takes care of memory management, 
thus the caller does not care about memory issues). And secondly, I wanted a class that that would be easy to use. 
Remember, <code>CEnum</code>'s job is simply to create a list of file names.</p>

<p>So, here is how things work in <code>CEnum</code>:</p>
<ol>

<li>Constructor allocates two lists</li>
<li>After enumeration process ends, pointers to two lists are returned to calling function</li>
<li>Pointer(s) to list(s) are deleted in destructor of <code>CEnum</code></li>
</ol>

<p>This means that your enumeration list(s) will live only as long as is the life time of <code>CEnum</code> object that created it !!! 
You cannot pass the pointer to some other function. If you would like to do that, you either need to create a copy of the list, 
or comment out some (or all) delete statements in destructor.</p>

<p>Is this a bad design?
It just might be, but simplicity of use was my primary issue when I designed this class. 
I wanted to be able to enumerate directory in a single line (see example above), 
do something with those files (e.g. display them on the screen) and then to forget about <code>CEnum</code> object and all the lists it allocated.
If you want to do it 'properly' <code>CEnum</code> can be easily adapted to use lists allocated by client application, 
all that is needed is to add one more constructor and to comment out few lines in destructor.</p>

\n
<h3><a name="Classmembers6">Class Members</a></h3>

<p>This is the list of CEnum's public member varibales through which user can set desired options:</p>

<ul>
<li><strong>bRecursive</strong><br>Description: if true subdirectories will be enumerated too<br>Default value: false </li>

<li><strong>bFullPath</strong><br>Description: if true files will be enumerated using file's full path, otherwise list will contain file names only<br>Default value: false </li>

<li><strong>bNoCaseDirs</strong><br>Description: if true case will be ignored when searching directory (and only directory) names<br>Default value: false </li>

<li><strong>bNoCaseFiles</strong><br>Description: if true case will be ignored when searching file (and only file) names<br>Default value: false </li>

<li><strong>sIncPatternFiles</strong><br>Description: matching pattern
for files you wish to include in your search. Wild cards * and ? are
supported. If you have more than one search pattern, separate them with
semicolon.<br>Default value: empty string <br>Examples: 
<ul>
<li>"*.mp3;*.mp4" </li>
<li>"*.mp?" (same as first example) </li>
<li>"*.mp3;iron maid*;latest*" </li>
</ul>

Note that in case of Include patterns, empty string means "enumerate all", i.e. everything is included !! </li>

<li><strong>sExcPatternFiles</strong><br>Description: matching pattern
for files you wish to exclude from your search. Wild cards * and ? are
supported. If you have more that one search pattern, separate them with
semicolon.<br>Default value: empty string <br>Examples: 
<ul>
<li>"*.mp3;*.mp4" </li>
<li>"*.mp?" (same as first example) </li>
<li>"*.mp3;iron maid*;latest*" </li>
</ul>

Note that in case of Exclude patterns, empty string means "enumerate none", i.e. nothing is excluded!!<br>Also, in case of conflict, Exclude pattern has presedence over Include pattern. </li>

<li><strong>sIncPatternDirs</strong> same as <strong>sIncPatternFiles</strong> above, just for directories. </li>

<li><strong>sExcPatternDirs</strong> same as <strong>ExcPatternFiles</strong> above, just for directories. </li>
</ul>

\n

<h3><a name="STLlistvs.MFCcollectionclasses7">STL List vs. MFC Collection Classes</a></h3>

<p><code>CEnum</code> can give you a variety of lists:</p>

<pre lang="c++">// ... show all containers CEnum can return

CEnum enumerator(_T("C:\\"));

list&lt;string&gt; * pSTLlistFiles   = enumerator.GetFiles();
list&lt;string&gt; * pSTLlistDirs    = enumerator.GetDirs();

StringList   * pMFClistFiles   = enumerator.GetFilesAsCStringList();
StringList   * pMFClistDirs    = enumerator.GetDirsAsCStringList();

StringArray  * pMFCArrayFiles  = enumerator.GetFilesAsCStringArray();
StringArray  * pMFCArrayDirs   = enumerator.GetDirsAsCStringArray();</pre>

<p>Since <code>CEnum</code> was written using STL, the only two lists created during the execution are two lists returned by <code>GetDirs()</code> and <code>GetFiles()</code> functions. All four MFC containers (two <code>CStringArrays</code> and two <code>CStringLists</code>) are created only when you call conversion functions (<code>GetFilesAs...</code> and <code>GetDirsAs...</code>).
In fact, all MFC related stuff is hidden behind preprocessor directives
and is by default inactive (i.e. it does not compile). If you need this
functionality, then just uncomment <code>//# define MFC</code> line and recompile. </p>

\n

That is it, I wish you a happy, happy enumerating :-).

*/