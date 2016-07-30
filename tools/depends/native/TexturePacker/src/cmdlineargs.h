#ifndef CMDLINEARGS_H
#define CMDLINEARGS_H

/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#ifdef TARGET_POSIX
#include "PlatformDefs.h"
#include "xwinapi.h"
typedef LPSTR PSZ;
#define _snprintf snprintf
#else
#include <windows.h>
#endif
#include <vector>
#include <string>
#include <iostream>

class CmdLineArgs : public std::vector<char*>
{
public:
    CmdLineArgs ()
    {
        // Save local copy of the command line string, because
        // ParseCmdLine() modifies this string while parsing it.
        PSZ cmdline = GetCommandLine();
        m_cmdline = new char [strlen (cmdline) + 1];
        if (m_cmdline)
        {
            strcpy (m_cmdline, cmdline);
            ParseCmdLine(); 
        } else {
#ifdef TARGET_POSIX
          delete[] cmdline;
#endif
        }
    }

    CmdLineArgs (const int argc, const char **argv)
    {
      std::string cmdline;
      std::cout << cmdline.c_str() << std::endl;
#ifdef TARGET_POSIX
      cmdline = "\"";
#endif
      for (int i = 0 ; i<argc ; i++)
      {
        cmdline += argv[i];
        if ( i != (argc-1) )
        {
#ifdef TARGET_POSIX
          cmdline += "\" \"";
#else
          cmdline += " ";
#endif
        }
      }
#ifdef TARGET_POSIX
      cmdline += "\"";
#endif
      m_cmdline = new char [cmdline.length() + 1];
      if (m_cmdline)
      { 
          //memset( m_cmdline, 0, cmdline.length() + 1);
          strcpy(m_cmdline, cmdline.c_str());
          std::cout << cmdline.c_str() << std::endl;
          ParseCmdLine();
      }
    }

    ~CmdLineArgs()
    {
        delete[] m_cmdline;
    }

private:
    PSZ m_cmdline; // the command line string

    ////////////////////////////////////////////////////////////////////////////////
    // Parse m_cmdline into individual tokens, which are delimited by spaces. If a
    // token begins with a quote, then that token is terminated by the next quote
    // followed immediately by a space or terminator.  This allows tokens to contain
    // spaces.
    // This input string:     This "is" a ""test"" "of the parsing" alg"o"rithm.
    // Produces these tokens: This, is, a, "test", of the parsing, alg"o"rithm
    ////////////////////////////////////////////////////////////////////////////////
    void ParseCmdLine ()
    {
        enum { TERM  = '\0',
               QUOTE = '\"' };

        bool bInQuotes = false;
        PSZ pargs = m_cmdline;

        std::cout << "Before Loop" << std::endl;
        while (*pargs)
        {
            std::cout << "In Loop" << std::endl;
            while (isspace (*pargs))        // skip leading whitespace
                pargs++;

            bInQuotes = (*pargs == QUOTE);  // see if this token is quoted

            if (bInQuotes)                  // skip leading quote
                pargs++; 

            push_back (pargs);              // store position of current token

            // Find next token.
            // NOTE: Args are normally terminated by whitespace, unless the
            // arg is quoted.  That's why we handle the two cases separately,
            // even though they are very similar.
            if (bInQuotes)
            {
                // find next quote followed by a space or terminator
                while (*pargs && 
                      !(*pargs == QUOTE && (isspace (pargs[1]) || pargs[1] == TERM)))
                    pargs++;
                if (*pargs)
                {
                    *pargs = TERM;  // terminate token
                    if (pargs[1])   // if quoted token not followed by a terminator
                        pargs += 2; // advance to next token
                }
            }
            else
            {
                // skip to next non-whitespace character
                while (*pargs && !isspace (*pargs)) 
                    pargs++;
                if (*pargs && isspace (*pargs)) // end of token
                {
                   *pargs = TERM;    // terminate token
                    pargs++;         // advance to next token or terminator
                }
            }
        } // while (*pargs)
        std::cout << "After Loop" << std::endl;
    } // ParseCmdLine()
}; // class CmdLineArgs


#endif // CMDLINEARGS_H
