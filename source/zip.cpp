#include <3ds.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <minizip/unzip.h>
#include <sys/stat.h>
#include "extract_zip.hpp"
#define dir_delimter '/'
#define MAX_FILENAME 512
#define READ_SIZE 8192
#include <cstring>
using namespace std; 
int zip_extract(string location,string extract_location)
{
	cout<<"File exatraction starting"<<endl;
    // Open the zip file
    unzFile *zipfile =(void**)unzOpen64(location.c_str());
    if ( zipfile == NULL )
    {
        cout<<"Not found"<<endl;
        return -1;
    }

    // Get info about the zip file
    unz_global_info64 global_info;
    if ( unzGetGlobalInfo64( zipfile, &global_info ) != UNZ_OK )
    {
        cout<<"Could not read file global info"<<endl;
        unzClose( zipfile );
        return -1;
    }

    // Buffer to hold data read from the zip file.
    char read_buffer[ READ_SIZE ];

    // Loop to extract all files
    uLong i;
    for ( i = 0; i < global_info.number_entry; ++i )
    {
        // Get info about current file.
        unz_file_info64 file_info;
        char filename[ MAX_FILENAME ];
        if ( unzGetCurrentFileInfo64(
            zipfile,
            &file_info,
            filename,
            MAX_FILENAME,
            NULL, 0, NULL, 0 ) != UNZ_OK )
        {
            cout<<"could not read file info"<<endl;
            unzClose( zipfile );
            return -1;
        }
		string ext = (filename);
		ext = extract_location + ext;
        // Check if this entry is a directory or file.
        const size_t filename_length = strlen( filename );
        if ( filename[ filename_length-1 ] == dir_delimter )
        {
            // Entry is a directory, so create it.
            cout<<"dir:"<<ext<<endl;
            mkdir(ext.c_str() ,0777);
        }
        else
        {
            // Entry is a file, so extract it.
            cout<<"file:"<<ext<<endl;
            if ( unzOpenCurrentFile( zipfile ) != UNZ_OK )
            {
                cout<<"could not open file"<<endl;
                unzClose( zipfile );
                return -1;
            }
			
            // Open a file to write out the data.
            FILE *out = fopen(ext.c_str(), "wb");
            if ( out == NULL )
            {
                cout<<"could not open destination file"<<endl;
                unzCloseCurrentFile( zipfile );
                unzClose( zipfile );
                return -1;
            }

            int error = UNZ_OK;
            do    
            {
                error = unzReadCurrentFile( zipfile, read_buffer, READ_SIZE );
                if ( error < 0 )
                {
                    cout<<"error :"<<error<<endl;
                    unzCloseCurrentFile( zipfile );
                    unzClose( zipfile );
                    return -1;
                }

                // Write data to file.
                if ( error > 0 )
                {
                    fwrite( read_buffer, error, 1, out ); // You should check return of fwrite...
                }
            } while ( error > 0 );

            fclose( out );
        }

        unzCloseCurrentFile( zipfile );

        // Go the the next entry listed in the zip file.
        if ( ( i+1 ) < global_info.number_entry )
        {
            if ( unzGoToNextFile( zipfile ) != UNZ_OK )
            {
                cout<<"Cound not read next file\n"<<endl;
                unzClose( zipfile );
                return -1;
            }
        }
    }
    unzClose( zipfile );
	return 0;
}
