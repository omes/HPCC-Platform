/*##############################################################################

    Copyright (C) 2011 HPCC Systems.

    All rights reserved. This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
############################################################################## */

#ifndef _ESPWIZ_FileSpray_HPP__
#define _ESPWIZ_FileSpray_HPP__

#include "ws_fs_esp.ipp"
#include "msgbuilder.hpp"
#include "jthread.hpp"
#include "dfuwu.hpp"

class Schedule : public Thread
{
    IEspContainer* m_container;
public:
    virtual int run();
    virtual void setContainer(IEspContainer * container)
    {
        m_container = container;
    }
};

class CFileSprayEx : public CFileSpray
{
public:
    virtual void init(IPropertyTree *cfg, const char *process, const char *service);
    virtual void setContainer(IEspContainer * container)
    {
        CFileSpray::setContainer(container);
        m_sched.setContainer(container);
    }

    virtual bool onEchoDateTime(IEspContext &context, IEspEchoDateTime &req, IEspEchoDateTimeResponse &resp)
    {
        resp.setResult("0000-00-00T00:00:00");
        return true;
    }

    virtual bool onShowResult(IEspContext &context, IEspShowResultRequest &req, IEspShowResultResponse &resp)
    {
        resp.setResult(req.getResult());
        return true;
    }

    virtual bool onDFUWUSearch(IEspContext &context, IEspDFUWUSearchRequest & req, IEspDFUWUSearchResponse & resp);
    virtual bool onGetDFUWorkunits(IEspContext &context, IEspGetDFUWorkunits &req, IEspGetDFUWorkunitsResponse &resp);
    virtual bool onGetDFUWorkunit(IEspContext &context, IEspGetDFUWorkunit &req, IEspGetDFUWorkunitResponse &resp);
    virtual bool onCreateDFUWorkunit(IEspContext &context, IEspCreateDFUWorkunit &req, IEspCreateDFUWorkunitResponse &resp);
    virtual bool onUpdateDFUWorkunit(IEspContext &context, IEspUpdateDFUWorkunit &req, IEspUpdateDFUWorkunitResponse &resp);
    virtual bool onDeleteDFUWorkunits(IEspContext &context, IEspDeleteDFUWorkunits &req, IEspDeleteDFUWorkunitsResponse &resp);
    virtual bool onDeleteDFUWorkunit(IEspContext &context, IEspDeleteDFUWorkunit &req, IEspDeleteDFUWorkunitResponse &resp);
    virtual bool onSubmitDFUWorkunit(IEspContext &context, IEspSubmitDFUWorkunit &req, IEspSubmitDFUWorkunitResponse &resp);
    virtual bool onAbortDFUWorkunit(IEspContext &context, IEspAbortDFUWorkunit &req, IEspAbortDFUWorkunitResponse &resp);
    virtual bool onGetDFUExceptions(IEspContext &context, IEspGetDFUExceptions &req, IEspGetDFUExceptionsResponse &resp);

    virtual bool onSprayFixed(IEspContext &context, IEspSprayFixed &req, IEspSprayFixedResponse &resp);
    virtual bool onSprayVariable(IEspContext &context, IEspSprayVariable &req, IEspSprayResponse &resp);
    virtual bool onReplicate(IEspContext &context, IEspReplicate &req, IEspReplicateResponse &resp);
    virtual bool onDespray(IEspContext &context, IEspDespray &req, IEspDesprayResponse &resp);
    virtual bool onCopy(IEspContext &context, IEspCopy &req, IEspCopyResponse &resp);
    virtual bool onRename(IEspContext &context, IEspRename &req, IEspRenameResponse &resp);
    virtual bool onDFUWUFile(IEspContext &context, IEspDFUWUFileRequest &req, IEspDFUWUFileResponse &resp);
    virtual bool onFileList(IEspContext &context, IEspFileListRequest &req, IEspFileListResponse &resp);
    virtual bool onDFUWorkunitsAction(IEspContext &context, IEspDFUWorkunitsActionRequest &req, IEspDFUWorkunitsActionResponse &resp);
    virtual bool onDfuMonitor(IEspContext &context, IEspDfuMonitorRequest &req, IEspDfuMonitorResponse &resp);
    virtual bool onGetDFUProgress(IEspContext &context, IEspProgressRequest &req, IEspProgressResponse &resp);
    virtual bool onOpenSave(IEspContext &context, IEspOpenSaveRequest &req, IEspOpenSaveResponse &resp);
    virtual bool onDropZoneFiles(IEspContext &context, IEspDropZoneFilesRequest &req, IEspDropZoneFilesResponse &resp);
    virtual bool onDeleteDropZoneFiles(IEspContext &context, IEspDeleteDropZoneFilesRequest &req, IEspDFUWorkunitsActionResponse &resp);

protected:
    StringBuffer m_QueueLabel;
    StringBuffer m_MonitorQueueLabel;
    StringBuffer m_RootFolder;
    Schedule m_sched;
    Owned<IPropertyTree> directories;
    
    void addToQueryString(StringBuffer &queryString, const char *name, const char *value);
    int doFileCheck(const char* mask, const char* netaddr, const char* osStr, const char* path);
    virtual bool doCopyForRoxie(IEspContext &context,   const char * srcName, const char * srcDali, const char * srcUser, 
        const char * srcPassword, const char * dstName, const char * destCluster, bool compressed, bool overwrite, bool supercopy, 
        DFUclusterPartDiskMapping val, StringBuffer baseDir, StringBuffer fileMask, IEspCopyResponse &resp);
    void getInfoFromSasha(IEspContext &context, const char *sashaServer, const char* wuid, IEspDFUWorkunit *info);
    bool getArchivedWUInfo(IEspContext &context, IEspGetDFUWorkunit &req, IEspGetDFUWorkunitResponse &resp);
    bool GetArchivedDFUWorkunits(IEspContext &context, IEspGetDFUWorkunits &req, IEspGetDFUWorkunitsResponse &resp);
    bool getDropZoneFiles(IEspContext &context, const char* netaddr, const char* osStr, const char* path, IEspDropZoneFilesRequest &req, IEspDropZoneFilesResponse &resp);
    bool ParseLogicalPath(const char * pLogicalPath, StringBuffer &title);
    bool ParseLogicalPath(const char * pLogicalPath, const char* cluster, StringBuffer &folder, StringBuffer &title, StringBuffer &defaultFolder, StringBuffer &defaultReplicateFolder);
};

#endif //_ESPWIZ_FileSpray_HPP__
