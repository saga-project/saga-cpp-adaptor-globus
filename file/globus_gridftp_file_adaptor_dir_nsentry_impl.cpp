//  Copyright (c) 2006-2008 Ole Weidner (oweidner@cct.lsu.edu)
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>

#include <boost/assert.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <saga/saga/url.hpp>
#include <saga/saga/exception.hpp>
#include <saga/saga/adaptors/task.hpp>

#include <saga/impl/config.hpp>

#include "globus_gridftp_file_adaptor_dir.hpp"
#include "globus_gridftp_file_adaptor_connection.hpp"

using namespace globus_gridftp_file_adaptor;

///////////////////////////////////////////////////////////////////////////////
//
void dir_cpi_impl::sync_get_url  (saga::url & url)
{
    saga::url u;
    {
        instance_data InstanceData (this);
        u = InstanceData->location_;
        this->throw_if_local(InstanceData->location_);
        this->check_if_open ("dir_cpi_impl::sync_get_url", InstanceData->location_);
    }
    
    // complete url
    if (u.get_scheme().empty())
        u.set_scheme("gridftp");
    
    url = u.get_url();
}

///////////////////////////////////////////////////////////////////////////////
//
void dir_cpi_impl::sync_get_cwd (saga::url & cwd)
{
    saga::url u;
    {
        instance_data InstanceData (this);
        u = InstanceData->location_;
        this->throw_if_local(InstanceData->location_);
        this->check_if_open ("dir_cpi_impl::sync_get_cwd", InstanceData->location_);
    }
    
    std::string u_str(u.get_url());
    std::string::size_type idx = u_str.rfind("/.");
    
    if( idx == u_str.size()-2 )
    {
       u_str = u_str.substr(0, idx);
    }
    
    cwd = saga::url(u_str);
}

///////////////////////////////////////////////////////////////////////////////
//
void dir_cpi_impl::sync_get_name (saga::url & name)
{
    saga::url u;
    {
      instance_data InstanceData (this);
      u = InstanceData->location_;
      this->throw_if_local(InstanceData->location_);
      this->check_if_open ("dir_cpi_impl::sync_get_name", InstanceData->location_);
    }
  
    namespace fs = boost::filesystem;
    
    this->sync_get_cwd(u); // to get rid of the /. at the end...
    
    fs::path path(u.get_path());
    std::string path_str(u.get_path());
        
    if( !path.has_root_path() )
        path = fs::path("/"+path_str);
    
    path_str = path.string();
    std::string::size_type idx = path_str.rfind("/");
    
    ( idx == 0 ) ? path = fs::path(path_str.substr(1, path_str.size()-1)) : 
    path = fs::path(path_str.substr(idx+1, path_str.size()-1));
    
    if( path.string().size() == 0 ) path = fs::path("/");
    
    name = path.string();
}

///////////////////////////////////////////////////////////////////////////////
//
void dir_cpi_impl::sync_is_dir (bool & is_dir)
{
    saga::url u;
    {
      instance_data InstanceData (this);
      u = InstanceData->location_;
      this->throw_if_local(InstanceData->location_);
      this->check_if_open ("dir_cpi_impl::sync_is_dir", InstanceData->location_);
    }
  
    is_dir = true;
}

///////////////////////////////////////////////////////////////////////////////
//
void dir_cpi_impl::sync_is_entry (bool & is_entry)
{
    saga::url u;
    {
      instance_data InstanceData (this);
      u = InstanceData->location_;
      this->throw_if_local(InstanceData->location_);
      this->check_if_open ("dir_cpi_impl::sync_is_entry", InstanceData->location_);
    }
  
    is_entry = false; 
}

///////////////////////////////////////////////////////////////////////////////
//
void dir_cpi_impl::sync_is_link (bool & is_link)
{
    adaptor_data_t AdaptorData(this);
    directory_instance_data_t InstanceData (this);

    this->throw_if_local(InstanceData->location_);
    this->check_if_open ("dir_cpi_impl::sync_is_link", InstanceData->location_);

    GridFTPConnection * ConnectionHandle = 
    AdaptorData->getConnectionHandleForURL(InstanceData->location_, write_log_, logfile_loc_);  
    
    bool test = false;
    
    try 
    {
        test = ConnectionHandle->is_symlink(InstanceData->location_.get_url());
    } 
    catch( globus_gridftp_file_adaptor::exception const &e)
    {
        error_package ep = globus_gridftp_file_adaptor
        ::error_default_redirect(e, InstanceData->location_.get_url());
        SAGA_OSSTREAM strm;
        strm << "Could not check if [" << InstanceData->location_ << "] is a (sym-)link." 
        << ep.error_text;
        SAGA_ADAPTOR_THROW(SAGA_OSSTREAM_GETSTRING(strm), ep.saga_error);
    }
    
    is_link = test;
}

///////////////////////////////////////////////////////////////////////////////
//
void dir_cpi_impl::sync_read_link (saga::url & target)
{
    adaptor_data_t AdaptorData(this);
    directory_instance_data_t InstanceData (this);
  
    this->throw_if_local(InstanceData->location_);
    this->check_if_open ("dir_cpi_impl::sync_read_link", InstanceData->location_);    
    
    GridFTPConnection * ConnectionHandle = 
    AdaptorData->getConnectionHandleForURL(InstanceData->location_, write_log_, logfile_loc_);    
    
    try 
    {
        target = ConnectionHandle->read_symlink(InstanceData->location_.get_url());
    } 
    catch( globus_gridftp_file_adaptor::exception const &e)
    {
        error_package ep = globus_gridftp_file_adaptor
        ::error_default_redirect(e, InstanceData->location_.get_url());
        SAGA_OSSTREAM strm;
        strm << "Could not read (sym-)link [" << InstanceData->location_ << "]. " 
             << ep.error_text;
        SAGA_ADAPTOR_THROW(SAGA_OSSTREAM_GETSTRING(strm), ep.saga_error);  
    }
}

///////////////////////////////////////////////////////////////////////////////
//
void dir_cpi_impl::sync_link (saga::impl::void_t & ret, saga::url dest, int flags)
{
    adaptor_data_t AdaptorData(this);
    directory_instance_data_t InstanceData (this);
  
    this->throw_if_local(InstanceData->location_);
    this->check_if_open ("dir_cpi_impl::sync_link", InstanceData->location_);
	
    // uses the ns::dir implementation
    this->sync_link (ret, InstanceData->location_, dest, flags);
}

///////////////////////////////////////////////////////////////////////////////
//
void dir_cpi_impl::sync_copy (saga::impl::void_t & ret, saga::url dest, int flags)
{    
    adaptor_data_t AdaptorData(this);
    directory_instance_data_t InstanceData (this);
  
    this->check_if_open ("dir_cpi_impl::sync_copy", InstanceData->location_);
	
    // uses the ns::dir implementation
    this->sync_copy (ret, InstanceData->location_, dest, flags);
}

///////////////////////////////////////////////////////////////////////////////
//
void dir_cpi_impl::sync_move (saga::impl::void_t & ret, saga::url dest, int flags)
{
    adaptor_data_t AdaptorData(this);
    directory_instance_data_t InstanceData (this);
  
    this->check_if_open ("dir_cpi_impl::sync_move", InstanceData->location_);
	
    // uses the ns::dir implementation
    this->sync_move (ret, InstanceData->location_, dest, flags);
}

///////////////////////////////////////////////////////////////////////////////
//
void dir_cpi_impl::sync_remove (saga::impl::void_t & ret, int flags)
{
    adaptor_data_t AdaptorData(this);
    directory_instance_data_t InstanceData (this);
  
    this->throw_if_local(InstanceData->location_);
    this->check_if_open ("dir_cpi_impl::sync_remove", InstanceData->location_);
	
    // uses the ns::dir implementation
    this->sync_remove (ret, InstanceData->location_, flags);
	
}

///////////////////////////////////////////////////////////////////////////////
//
void dir_cpi_impl::sync_close (saga::impl::void_t & ret, double timeout)
{
    is_open_ = false;
}
