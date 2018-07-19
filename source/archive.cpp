#include "archive.h"

static int copy_data(struct archive *ar, struct archive *aw)
{
  int r;
  const void *buff;
  size_t size;
  int64_t offset;

  for (;;) {
    r = archive_read_data_block(ar, &buff, &size, &offset);
    if (r == ARCHIVE_EOF)
      return (ARCHIVE_OK);
    if (r < ARCHIVE_OK)
      return r;
    r = archive_write_data_block(aw, buff, size, offset);
    if (r < ARCHIVE_OK) {
      //printf("%s\n", archive_error_string(aw));
      return r;
    }
  }
}
void archive::extract(std::string filename)
{
    /* Select which attributes we want to restore. */ 
    this->m_flags = ARCHIVE_EXTRACT_PERM;
		this->m_flags |= ARCHIVE_EXTRACT_ACL;
		this->m_flags |= ARCHIVE_EXTRACT_FFLAGS;
  	this->m_a = archive_read_new();
  	archive_read_support_format_all(this->m_a);
		this->m_ext = archive_write_disk_new();
  	archive_write_disk_set_options(this->m_ext, this->m_flags);
  	//archive_write_disk_set_standard_lookup(this->m_ext  	
		if ((this->m_r = archive_read_open_filename(this->m_a, filename.c_str(), 10240)))
    { sprintf(this->m_errorBuffer, "Couldn't find file"); return; }
  	for (;;) 
	{
    	this->m_r = archive_read_next_header(this->m_a, &this->m_entry);
    	if (this->m_r == ARCHIVE_EOF)
      		break;
    	if (this->m_r < ARCHIVE_OK)
      		{ sprintf(this->m_errorBuffer, "%s", archive_error_string(this->m_a)); return; }
    	if (this->m_r < ARCHIVE_WARN)
        	return;
    	this->m_r = archive_write_header(this->m_ext, this->m_entry);
    	if (this->m_r < ARCHIVE_OK)
      		{ sprintf(this->m_errorBuffer, "%s", archive_error_string(this->m_ext)); return; }
    	
		else if (archive_entry_size(this->m_entry) > 0) 
		{
      		this->m_r = copy_data(this->m_a, this->m_ext);
			if (this->m_r < ARCHIVE_OK)
        		{ sprintf(this->m_errorBuffer, "%s", archive_error_string(this->m_ext)); return; }
      		if (this->m_r < ARCHIVE_WARN)
        		return;
    	}
   		this->m_r = archive_write_finish_entry(this->m_ext);    	
		if (this->m_r < ARCHIVE_OK)
      		{ sprintf(this->m_errorBuffer, "%s", archive_error_string(this->m_ext)); return; }
    	if (this->m_r < ARCHIVE_WARN)
			return;
  	}
	archive_read_close(this->m_a);
	archive_read_free(this->m_a);
	archive_write_close(this->m_ext);
	archive_write_free(this->m_ext);
	return;
}