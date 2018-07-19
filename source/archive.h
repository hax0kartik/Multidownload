#include <string>
#include <archive.h>
#include <archive_entry.h>

class archive
{
    public:
        void extract(std::string filename);
        std::string getError(){ return std::string(this->m_errorBuffer); }
    private:
        char m_errorBuffer[1024];
        struct archive *m_a;
        struct archive *m_ext;
        struct archive_entry *m_entry;
        int m_flags;
        int m_r;

};