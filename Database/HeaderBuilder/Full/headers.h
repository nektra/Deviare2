//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//
// NON-BASE PART BEGINS HERE
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

#include <ks.h>
#include "dsound.h"
#include "d3d9.h"
//#include "fwptypes.h"
//#include "iketypes.h"
#include "NktMedia.h"


//#include <MAPIDefS.h>

#include <AccCtrl.h>
#include <AclAPI.h>
#include <AclUI.h>
#include <activaut.h>
#include <activdbg.h>
#include <activecf.h>
#include <ActiveDS.h>
#include <activprof.h>
#include <ActivScp.h>
#include <adc.h>
#include <adhoc.h>
/*
#include <Admex.h>
#include <adoctint.h>
#include <adodef.h>
//#include <adogpool.h>
#include <adoguids.h>
#include <adoid.h>
#include <adoint.h>
#include <adojet.h>
#include <adomd.h>
*/
#include <AdsDb.h>
#include <AdsErr.h>
#include <AdsHlp.h>
#include <ADSIid.h>
#include <Adsnms.h>
#include <AdsProp.h>
#include <Adssts.h>
#include <AdtGen.h>
#include <advpub.h>
#include <AF_Irda.h>
/*
#include <AgtCtl.h>
#include <AgtErr.h>
#include <AgtSvr.h>
*/
//#include <ahadmin.h>
#include <alg.h>
#include <amaudio.h>
#include <amparse.h>
#include <amstream.h>
#include <amva.h>
#include <amvideo.h>
/*
#include <amxmlgraphbuilder.h>
*/
#include <anchorsyncdeviceservice.h>
#include <appcompatapi.h>
#include <appmgmt.h>
/*
#include <AqAdmTyp.h>
*/
#include <asferr.h>
#include <asptlb.h>
#include <atacct.h>
/*
#include <AtalkWsh.h>
#include <atscpsipparser.h>
#include <atsmedia.h>
*/
#include <audevcod.h>
#include <AudioAPOTypes.h>
#include <Audioclient.h>
#include <AudioEngineEndpoint.h>
#include <audiopolicy.h>
#include <audiosessiontypes.h>
#include <austream.h>
/*
#include <authen.h>
*/
#include <Authif.h>
#include <AuthZ.h>
#include <aux_ulib.h>
#include <AviFmt.h>
#include <aviriff.h>
#include <avrfsdk.h>
#include <avrt.h>
/*
#include <axextendenums.h>
*/
#include <azroles.h>
#include <BaseTsd.h>
#include <BaseTyps.h>
#include <BatClass.h>
#include <bcrypt.h>
/*
#include <bdaiface.h>
#include <bdaiface_enums.h>
#include <bdatypes.h>
#include <bdamedia.h>
#include <bdatif.h>
*/
#include <BiDiSpl.h>
#include <Bits.h>
#include <bits1_5.h>
#include <bits2_0.h>
#include <bits2_5.h>
#include <Bits3_0.h>
#include <bits4_0.h>
#include <bitscfg.h>
#include <BitsMsg.h>
#include <BluetoothAPIs.h>
/*
#include <bridgedeviceservice.h>
*/
#include <bthdef.h>
#include <bthsdpdef.h>
#include <calendardeviceservice.h>
#include <callobj.h>
#include <capi.h>
#include <casetup.h>
/*
#include <CChannel.h>
*/
#include <CdErr.h>
/*
#include <CDONTS.h>
#include <CDOSys.h>
*/
#include <CDOSysErr.h>
#include <CDOSysStr.h>
#include <CeLib.h>
#include <CertAdm.h>
#include <CertBase.h>
#include <CertBCli.h>
#include <CertCli.h>
#include <CertEnc.h>
#include <certenroll.h>
#include <CertExit.h>
#include <Certif.h>
#include <CertMod.h>
#include <CertPol.h>
#include <CertPolEng.h>
#include <certreqd.h>
#include <CertSrv.h>
#include <CertView.h>
#include <cfg.h>
#include <cfgmgr32.h>
#include <CGuid.h>
/*
#include <ChanMgr.h>
#include <ChPtrArr.h>
#include <ChStrArr.h>
#include <ChString.h>
*/
#include <CiError.h>
#include <clfs.h>
/*
#include <clfslsn.h>
*/
#include <clfsmgmt.h>
#include <clfsmgmtw32.h>
#include <clfsw32.h>
#include <clrdata.h>
#include <CluAdmEx.h>
#include <cmdtree.h>
#include <CmnQuery.h>
#include <codecapi.h>
/*
#include <colldispid.h>
*/
#include <ColorDlg.h>
#include <ComAdmin.h>
#include <ComCat.h>
#include <comlite.h>
#include <CommCtrl.h>
#include <CommDlg.h>
#include <commoncontrols.h>
/*
#include <CompObj.h>
*/
#include <contactdeviceservice.h>
#include <contentpartner.h>
#include <control.h>
#include <cor.h>
#include <cordebug.h>
#include <CorError.h>
#include <CorHdr.h>
/*
#include <corhlpr.h>
*/
#include <corprof.h>
#include <corpub.h>
#include <corrEngine.h>
//#include <corsym.h>
#include <cper.h>
#include <cperguid.h>
#include <Cpl.h>
#include <Cplext.h>
#include <credentialprovider.h>
#include <credssp.h>
#include <cryptdlg.h>
#include <cryptuiapi.h>
#include <cryptxml.h>
#include <cscapi.h>
#include <cscobj.h>
#include <ctffunc.h>
#include <ctfspui.h>
#include <ctfutb.h>
#include <ctxtcall.h>
#include <CustCntl.h>
/*
#include <CustomAw.h>
#include <d2d1.h>
#include <d2d1helper.h>
*/
#include <d2dBaseTypes.h>
#include <d2derr.h>
#include <d3d10_1.h> //dont use d3d10
#include <d3d10effect.h>
#include <d3d10misc.h>
#include <d3d10sdklayers.h>
#include <d3d10shader.h>
#include <D3D10_1shader.h>
#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <d3d9.h>
#include <d3d9caps.h>
#include <d3d9helper.h>
#include <d3d9types.h>
#include <D3DCommon.h>
#include <daogetrw.h>
/*
#include <datacontainer.h>
#include <datacontainerversion.h>
*/
#include <davclnt.h>
/*
#include <dbdao.h>
*/
#include <dbdaoerr.h>
#include <dbdaoid.h>
#include <dbdaoint.h>
#include <DbgEng.h>
/*
#include <DbgHelp.h> //seems a copy of imagehlp
*/
#include <DbgProp.h>
#include <dbnetlib.h>
#undef NETERR	
#undef TIMEINT
#undef RETCODE
#undef IOINT
#include <Dbt.h>
#include <dciddi.h>
#include <dciman.h>
#include <DCommon.h>
#include <Dde.h>
#include <Ddeml.h>
#include <ddraw.h>
/*
#include <ddrawgdi.h>
*/
#include <ddstream.h>
#include <DeleteBrowsingHistory.h>
#include <devguid.h>
#include <DeviceCategories.h>
#include <deviceservices.h>
#include <devicetopology.h>
#include <DEVPKEY.H>
#include <devpropdef.h>
#include <DhcpCSdk.h>
#include <dhcpsapi.h>
#include <DhcpSSdk.h>
#include <dhcpv6csdk.h>
#include <Digitalv.h>
#include <Dimm.h>
#include <dinput.h>
#include <dinputd.h>
#include <diskguid.h>
/*
#include <Dispatch.h>
#include <Dispdib.h>
*/
#include <DispEx.h>
#include <Dlgs.h>
#include <dls1.h>
#include <dls2.h>
#include <dmdls.h>
#include <dmerror.h>
#include <dmksctrl.h>
#include <dmo.h>
#include <dmodshow.h>
#include <dmoimpl.h>
#include <dmoreg.h>
#include <dmort.h>
#include <dmusbuff.h>
#include <dmusicc.h>
#include <dmusics.h>
#include <DocObj.h>
#include <DocObjectService.h>
/*
#include <DomDid.h>
*/
#include <dot1x.h>
#include <DownloadMgr.h>
/*
#include <dpnathlp.h>
*/
#include <dpx.h>
#include <dpx1.h>
#include <driverspecs.h>
#include <drivinit.h>
#include <drmexternals.h>
#include <drt.h>
#include <DSAdmin.h>
#include <dsattrib.h>
#include <DSClient.h>
#include <dsconf.h>
#include <DsGetDC.h>
#include <DShow.h>
#include <dshowasf.h>
#include <DskQuota.h>
#include <dsound.h>
#include <DSQuery.h>
#include <DSRole.h>
#include <dssec.h>
#include <DtcHelp.h>
#include <dv.h>
/*
#include <dvbsiparser.h>
*/
#include <dvdevcod.h>
#include <dvdmedia.h>
/*
#include <DvObj.h>
*/
#include <dwmapi.h>
/*
#include <dwrite.h>
*/
#include <dxgi.h>
#include <dxgiformat.h>
#include <dxgitype.h>
/*
#include <dxtmpl.h>
*/
#include <dxva.h>
#include <dxva2api.h>
#include <dxva2swdev.h>
#include <dxva2trace.h>
#include <dxvahd.h>
#include <eaptypes.h>
#include <EapAuthenticatorActionDefine.h>
#include <eapauthenticatortypes.h>
#include <EapHostError.h>
#include <eaphostpeerconfigapis.h>
#include <EapHostPeerTypes.h>
#include <eappapis.h>
#include <eapmethodpeerapis.h>
#include <eapmethodtypes.h>
#include <eapmethodauthenticatorapis.h>
#include <edevdefs.h>
#include <effects.h>
#include <ehstorapi.h>
#include <ehstorextensions.h>
#include <ehstormsg.h>
#include <ElsCore.h>
#include <ElsSrvc.h>
#include <EmptyVC.h>
#include <encdec_enums.h> //no va encdec.h
#include <endpointvolume.h>
/*
#include <Error.h>
*/
#include <ErrorRep.h>
#include <errors.h>
#include <esent.h>
#include <evalcom2.h>
#include <evcode.h>
#include <EvColl.h>
/*
#include <event.h>
*/
#include <EventSys.h>
#include <evntcons.h>
#include <evntprov.h>
#include <Evntrace.h>
#include <evr.h>
#include <evr9.h>
/*
#include <ExchForm.h>
*/
#include <ExDisp.h>
#include <ExDispid.h>
/*
#include <exposeenums2managed.h>
#include <fastcall.h>
*/
#include <Tapi.h> //before winfax & faxdev
#include <FaxComEx.h> //#no va FaxCom
#include <FaxDev.h>
#include <FaxExt.h>
#include <FaxMmc.h>
#include <FaxRoute.h>
#include <Fci.h>
#include <Fdi.h>
#include <FileHC.h>
#include <Filter.h>
#include <filtereg.h>
#include <Filterr.h>
#include <fltdefs.h>
#include <fontsub.h>
//#include <FrQuery.h>
#include <fsrm.h>
#include <fsrmenums.h>
#include <fsrmerr.h>
#include <fsrmpipeline.h>
#include <fsrmquota.h>
#include <fsrmreports.h>
#include <fsrmscreen.h>
#include <fsrmtlb.h>
#include <ftpext.h>
/*
#include <FtsIface.h>
*/
#include <fullenumsyncdeviceservice.h>
#include <functiondiscovery.h>
#include <functiondiscoveryapi.h>
#include <functiondiscoverycategories.h>
#include <functiondiscoveryconstraints.h>
#include <functiondiscoveryerror.h>
#include <functiondiscoverykeys.h>
#include <FunctionDiscoveryKeys_devpkey.h>
#include <functiondiscoverynotification.h>
/*
#include <functiondiscoveryprovider.h>
#include <functiondiscoveryproviderhelper.h>
*/
#include <functiondiscoveryserviceids.h>
/*
#include <fusion.h>
//#include <FwCommon.h>
*/
#include <fwpmtypes.h>
#include <fwpmu.h>
#include <fwpstypes.h>
#include <fwpsu.h>
#include <fwptypes.h>
#include <fwpvi.h>
#include <fxsutility.h>
#include <gameux.h>
#include <gb18030.h>
#include <gchost.h>
#include <GdiPlus.h>
/*
#include <GdiPlusBase.h>
#include <GdiPlusBitmap.h>
#include <GdiPlusBrush.h>
#include <GdiPlusCachedBitmap.h>
#include <GdiPlusColor.h>
#include <GdiPlusColorMatrix.h>
#include <gdipluseffects.h>
#include <GdiPlusEnums.h>
#include <GdiPlusFlat.h>
#include <GdiPlusFont.h>
#include <GdiPlusFontCollection.h>
#include <GdiPlusFontFamily.h>
#include <GdiPlusGpStubs.h>
#include <GdiPlusGraphics.h>
#include <GdiPlusHeaders.h>
#include <GdiPlusimageAttributes.h>
#include <GdiPlusImageCodec.h>
#include <GdiPlusImaging.h>
#include <GdiPlusInit.h>
#include <GdiPlusLineCaps.h>
#include <GdiPlusMatrix.h>
#include <GdiPlusMem.h>
#include <GdiPlusMetaFile.h>
#include <GdiPlusMetaHeader.h>
#include <GdiPlusPath.h>
#include <GdiPlusPen.h>
#include <GdiPlusPixelFormats.h>
#include <GdiPlusRegion.h>
#include <GdiPlusStringFormat.h>
#include <GdiPlusTypes.h>
*/
#include <GenLex.h>
#include <GPEdit.h>
#include <gpmgmt.h>
#include <Guiddef.h>
#include <highlevelmonitorconfigurationapi.h>
#include <HlGuids.h>
#include <HlIface.h>
#include <HLink.h>
/*
#include <HostInfo.h>
*/
#include <htiface.h>
#include <htiframe.h>
/*
#include <HtmlGuid.h>
*/
#include <HtmlHelp.h>
#include <httpcompression.h>
#include <HttpExt.h>
#include <HttpFilt.h>
#include <httprequestid.h>
/*
#include <httpcach.h>
#include <http.h>
#include <httpserv.h>
*/
#include <httptrace.h>
#include <hwebcore.h>
#include <hxhelppaneproxy.h>
/*
#include <ia64reg.h>
*/
#include <IAccess.h>
/*
#include <IACOM.h>
*/
#include <Iadmext.h>
#include <Iadmw.h>
#include <Iads.h>
/*
#include <IAGuid.h>
#include <ICeeFileGen.h>
*/
#include <icftypes.h>
#include <ichannelcredentials.h>
#include <Icm.h>
#include <IPExport.h> //before icmpapi
#include <IcmpAPI.h>
#include <icontact.h>
#include <icontactproperties.h>
#include <icrsint.h>
#include <icwcfg.h>
#include <IdentityCommon.h>
#include <IdentityProvider.h>
#include <IdentityStore.h>
/*
#include <Idf.h>
*/
#include <IDispIds.h>
#include <IDLMULTI.H>
#include <IEDial.h>
#include <IEObj.h>
#include <iepmapi.h>
#include <IEProcess.h>
#include <ieverp.h>
#include <iextag.h>
#include <ifdef.h>
#include <ifmib.h>
/*
#include <IIIS.h>
*/
interface IISApplicationPool;
#include <iiisext.h>
#include <IImgCtx.h>
#include <IIScnfg.h>
#include <iisrsta.h>
#include <iketypes.h>
#include <il21dec.h>
#include <ImageHlp.h>
#include <ImageTranscode.h>
#include <imapi.h>
#include <imapi2.h>
#include <imapi2error.h>
#include <imapi2fs.h>
#include <imapi2fserror.h>
#include <imapierror.h>
#include <Ime.h>
#include <Ime_cmodes.h>
#include <ImgUtil.h>
#include <Imm.h>
#include <in6addr.h>
#include <inaddr.h>
#include <indexsrv.h>
#include <InetReg.h>
#include <InetSDK.h>
#include <infocard.h>
//#include <infotech.h>
#include <InitGuid.h>
/*
#include <InitOID.h>
*/
#include <inked.h>
#include <InputScope.h>
//#include <Instance.h>
#include <intsafe.h>
#include <IntShCut.h>
#include <Ioevent.h>
#include <IPHlpApi.h>
#include <Ipifcons.h>
#include <Ipinfoid.h>
#include <ipmib.h>
#include <Iprtrmib.h>
#include <ipsectypes.h>
#include <IPTypes.h>
#include <IpxConst.h>
#include <Ipxrip.h>
#include <Ipxrtdef.h>
#include <Ipxsap.h>
#include <Ipxtfflt.h>
#include <IsGuids.h>
#include <isolation.h>
#include <IssPer16.h>
/*
#include <IssPErr.h>
*/
#include <ISysmon.h>
#include <ivalidator.h>
#include <ivehandler.h>
#include <Iwamreg.h>
#include <iwstdec.h>
/*
#include <i_cryptasn1tls.h>
*/
#include <journal.h>
#include <kernelspecs.h>
#include <KnownFolders.h>
#include <xprtdefs.h> //before ks
#include <Ks.h>
#include <KsGuid.h>
#include <KsMedia.h>
/*
#include <ksopmapi.h> use only opmapi.h
*/
#include <KsProxy.h>
/*
#include <ksuuids.h>
*/
#include <uuids.h> //includes ksuuids.h
#include <ktmtypes.h>
#include <ktmw32.h>
/*
#include <kxia64.h>
*/
#include <l2cmn.h>
#include <listeneradapter.h>
#include <LM.h>
#include <LMaccess.h>
#include <LMalert.h>
#include <LMAPIbuf.h>
#include <LMat.h>
#include <LMaudit.h>
#include <LMConfig.h>
#include <LMCons.h>
#include <LMDFS.h>
#include <LMErr.h>
#include <LMErrlog.h>
#include <LMJoin.h>
#include <LMMsg.h>
#include <LMRemUtl.h>
#include <LMRepl.h>
#include <LMServer.h>
#include <LMShare.h>
#include <LMSName.h>
#include <LMStats.h>
#include <LMSvc.h>
#include <LMUse.h>
#include <LMUseFlg.h>
#include <LMWksta.h>
#include <LoadPerf.h>
#include <locationapi.h>
#include <lowlevelmonitorconfigurationapi.h>
//#include <LpmApi.h> conflicts other libraries
#include <LsaLookup.h>
#include <LZExpand.h>
#include <madcapcl.h>
#include <magnification.h>
/*
#include <MailMsgProps.h>
*/
#include <manipulations.h>
#include <MAPI.h>
/*
#include <MAPICode.h>
#include <MAPIDbg.h>
#include <MAPIDefS.h>
#include <MAPIForm.h>
#include <MAPIGuid.h>
#include <MAPIHook.h>
#include <MAPINls.h>
#include <MAPIOID.h>
#include <MAPISPI.h>
#include <MAPITags.h>
#include <MAPIUtil.h>
#include <MAPIVal.h>
#include <MAPIWin.h>
#include <MAPIWz.h>
#include <MAPIX.h>
*/
#include <Wab.h> //conflicts mapi
#include <WabDefs.h>
#include <WabApi.h>
#include <WabCode.h>
#include <WabIab.h>
#include <WabMem.h>
#include <WabNot.h>
#include <WabTags.h>
#include <WabUtil.h>
#include <IMessage.h> //after mapi
#include <mbnapi.h>
#include <MciAvi.h>
#include <Mcx.h>
#include <Mdcommsg.h>
#include <Mddefw.h>
#include <MDhcp.h>
#include <Mdmsg.h>
#include <mediaerr.h>
#include <mediaobj.h>
#include <medparam.h>
#include <mergemod.h>
#include <messagedeviceservice.h>
#include <metadatadeviceservice.h>
//#include <metahost.h>
//#include <MethodCo.h>
#include <mfapi.h>
#include <Mferror.h>
#include <mfidl.h>
#include <mfmp2dlna.h>
#include <mfobjects.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <Mgm.h>
#include <MgmtAPI.h>
#include <micaut.h>
#include <Midles.h>
/*
#include <mileffects.h>
*/
#include <mimedisp.h>
#include <MimeInfo.h>
#include <mixerocx.h>
#include <MLang.h>
/*
#include <MMC.h>
*/
#include <MMCObj.h>
#include <mmdeviceapi.h>
#include <MMReg.h>
#include <mmstream.h>
#include <MMSystem.h>
#include <Mobsync.h>
/*
#include <Moniker.h>
*/
#include <mpconfig.h>
#include <mpeg2bits.h>
/*
#include <mpeg2data.h>
#include <mpeg2error.h>
#include <mpeg2psiparser.h>
*/
#include <mpeg2structs.h>
#include <mpegtype.h>
#include <Mprapi.h>
#include <MprError.h>
#include <Mq.h>
/*
#include <MqMail.h>
#include <MqOaI.h>
*/
#include <MSAAText.h>
#include <MSAcm.h>
#include <MSAcmDlg.h>
/*
#include <msado15.h>
#include <msasn1.h>
#include <msber.h>
*/
#include <mscat.h>
#include <MSChapp.h>
#include <MSClus.h>
#include <ClusApi.h>
#include <mscoree.h>
#include <msctf.h>
#include <msctfmonitorapi.h>
#include <msdadc.h>
#include <msdaguid.h>
/*
#include <MSDaIpp.h>
#include <MSDaIppEr.h>
*/
#include <msdaora.h>
#include <msdaosp.h>
#include <msdasc.h>
#include <msdasql.h>
#include <msdasql_interfaces.h>
#include <msdatsrc.h>
#include <msdelta.h>
#include <msdrm.h>
#include <msdrmdefs.h>
#include <msdrmerror.h>
#include <msdrmgetinfo.h>
#include <msdshape.h>
#include <msfeeds.h>
#include <msfeedsid.h>
/*
#include <MSFS.h>
*/
#include <MsHtmcid.h>
#include <MsHtmdid.h>
#include <MsHtmHst.h>
#include <MsHTML.h>
#include <Mshtmlc.h>
/*
#include <DHtmldid.h> //these below mshtml
#include <DHtmled.h>
#include <DHtmliid.h>
*/
#include <Msi.h>
#include <MsiDefs.h>
#include <msiehost.h>
#include <msiltcfg.h>
/*
#include <msimcntl.h>
#include <msimcsdk.h>
*/
#include <msinkaut.h>
#include <msinkaut15.h>
#include <MsiQuery.h>
/*
#include <msoav.h>
*/
#include <msopc.h>
#include <Msp.h>
/*
#include <MSPAB.h>
#include <MspAddr.h> clases c++ atl
#include <MspCall.h>
#include <Mspcoll.h>
#include <MspEnum.h>
#include <Msplog.h>
#include <MSPST.h>
#include <MspStrm.h>
#include <Mspterm.h>
#include <Mspthrd.h>
#include <Msptrmac.h>
#include <Msptrmar.h>
#include <Msptrmvc.h>
#include <Msputils.h>
*/
#include <msrdc.h>
#include <msremote.h>
#include <mssip.h>
#include <msstkppg.h>
#include <MSTask.h>
#include <MSTcpIP.h>
#include <MSTErr.h>
/*
#include <mstime.h>
#include <mstimeid.h>
#include <msvidctl.h>
*/
#include <mswmdm.h>
#include <MSWSock.h>
#include <MSWSockDef.h>
#include <MsXml.h>
#include <MsXml2.h>
#include <MsXml2DId.h>
#include <msxml6.h>
#include <MsXmlDId.h>
#include <mtpext.h>
#include <MtsAdmin.h>
/*
#include <ComSvcs.h>  //all the same
#include <Mtx.h>
#include <MtxAdmin.h>
#include <MtxAttr.h>
#include <Mtxdm.h>
#include <MtsEvents.h>
#include <MtsGrp.h>
#include <TxCtx.h>
*/
#include <muiload.h>
#include <MultiMon.h>
/*
#include <MultInfo.h>
*/
#include <mxdc.h>
#include <napcertrelyingparty.h>
#include <napcommon.h>
#include <napenforcementclient.h>
/*
#include <naperror.h>
*/
#include <napmanagement.h>
#include <napmicrosoftvendorids.h>
#include <napprotocol.h>
#include <napservermanagement.h>
#include <napsystemhealthagent.h>
#include <napsystemhealthvalidator.h>
#include <naptypes.h>
#include <naputil.h>
#include <natupnp.h>
#include <Nb30.h>
#include <ncrypt.h>
#include <ndattrib.h>
#include <ndfapi.h>
#include <ndhelper.h>
#include <ndr64types.h>
/*
#include <ndrtypes.h>
*/
#include <NetCon.h>
/*
#include <neterr.h>
*/
#include <netfw.h>
#include <netioapi.h>
#include <netlistmgr.h>
/*
//#include <netmon.h> the bh.h is the same
*/
#include <netprov.h>
#include <NetSh.h>
#include <newdev.h>
#include <nldef.h>
/*
#include <nmsupp.h>   depends on netmon
*/
#include <notesdeviceservice.h>
#include <Npapi.h>
#include <nsemail.h>
#include <nserror.h>
#include <NspAPI.h>
/*
#include <nsscore.h>
*/
#include <NtDDNdis.h>
#include <NtDDPSch.h>
//#include <ntddscsi.h> not needed
/*
#include <iscsierr.h> //before iscsidsc.h
#include <iscsidsc.h> //problems with winioctl
*/
#include <NtDsAPI.h>
/*
#include <NtDsBCli.h>
#include <NtDsBMsg.h>
#include <ntgdi.h>
*/
#include <NtLdap.h>
#include <NTMSAPI.h>
#include <NTMSMli.h>
#include <NTQuery.h>
#include <NTSDExts.h>
#include <NTSecAPI.h>
#include <NTSecPkg.h>
#include <ntstatus.h>
#include <ntverp.h>
#include <OAIdl.h>
#include <ObjBase.h>
#include <ObjectArray.h>
#include <ObjError.h>
#include <ObjIdl.h>
#include <ObjPath.h>
#include <ObjSafe.h>
#include <ObjSel.h>
#include <OCIdl.h>
#include <ocmm.h>
/*
#include <oestore6.h>
#include <ole.h>
*/
#include <Ole2.h> //not ole.h
#include <Ole2Ver.h>
#include <OleAcc.h>
#include <OleAuto.h>
#include <OleCtl.h>
/*
#include <OleCtlId.h>
*/
#include <oledb.h>
#include <oledbdep.h>
#include <oledberr.h>
/*
#include <oledbguid.h>
*/
#include <OleDlg.h>
/*
#include <OleIdl.h>
*/
#include <oleTx2xa.h>
#include <OPathLex.h>
#include <openservice.h>
#include <opmapi.h>
/*
#include <p2p.h>
#include <packoff.h>
#include <packon.h>
*/
#include <parser.h>
#include <PatchApi.h>
#include <PatchWiz.h>
/*
#include <Pbt.h>
*/
#include <PChannel.h>
#include <pciprop.h>
/*
#include <Pcrt32.h>
*/
#include <Pdh.h>
#include <PdhMsg.h>
#include <peninputpanel.h>
#include <perflib.h>
#include <PerHist.h>
#include <persist.h>
#include <PhotoAcquire.h>
#include <PhotoAcquireProperties.h>
#include <physicalmonitorenumerationapi.h>
#include <pla.h>
#include <playlist.h>
#include <PNPXAssoc.h>
#include <pnrpdef.h>
#include <pnrpns.h>
#include <Polarity.h>
#include <PopPack.h>
#include <PortableDevice.h>
#include <PortableDeviceApi.h>
#include <PortableDeviceConnectApi.h>
#include <PortableDeviceConnectImports.h>
#include <PortableDeviceTypes.h>
#include <powrprof.h>
#include <prnasnot.h>
#include <prntvpt.h>
#include <ProfInfo.h>
#include <propapi.h>
#include <PropIdl.h>
#include <propkey.h>
#include <propkeydef.h>
#include <propsys.h>
#include <propvarutil.h>
//#include <ProvExce.h>
//#include <Provider.h>
#include <PrSht.h>
#include <Psapi.h>
#include <PshPack1.h>
#include <PshPack2.h>
#include <PshPack4.h>
#include <PshPack8.h>
/*
#include <pstore.h>
*/
#include <qmgr.h>
#include <qnetwork.h>
#include <Qos.h>
#include <qos2.h>
#include <qosobjs.h>
#include <QosPol.h>
#include <Qossp.h>
#include <Query.h>
#include <Ras.h>
#include <RasDlg.h>
#include <Raseapif.h>
#include <RasError.h>
#include <RassHost.h>
#include <Ratings.h>
#include <rdpencomapi.h>
#include <Reason.h>
#include <recapis.h>
#include <recdefs.h>
/*
#include <RecGuids.h>
*/
#include <Reconcil.h>
#include <rectypes.h>
//#include <RefPtrCo.h>
#include <regbag.h>
#include <RegStr.h>
#include <REND.h>
#include <rendezvoussession.h>
#include <Resapi.h>
#include <RestartManager.h>
#include <Richedit.h>
#include <RichOle.h>
#include <ringtonedeviceservice.h>
//#include <Routprot.h>
#include <Rpc.h>
#include <RpcAsync.h>
#include <RpcDce.h>
#include <RpcDceP.h>
#include <RpcNdr.h>
#include <RpcNsi.h>
#include <RpcNsip.h>
#include <RpcNtErr.h>
#include <RpcProxy.h>
#include <Rpcsal.h>
#include <rpcssl.h>
#include <RrasCfg.h>
#include <rtccore.h>
#include <rtcerr.h>
#include <RTInfo.h>
#include <RtmV2.h>
/*
#include <RTSCOM.h>
*/
#include <Rtutils.h>
#include <sac.h>
/*
#include <sal_supp.h>
#include <sapi.h>
#include <sapi51.h>
*/
#include <sapi53.h> //dont include sapi & sapi51
/*
#include <sapiddk.h>
#include <sapiddk51.h>
*/
#include <sas.h>
#include <sbe.h>
#include <sberrors.h>
#include <sbtsv.h>
/*
#include <SCardDat.h> //all the same
#include <SCardErr.h>
#include <SCardMgr.h>
#include <SCardSrv.h>
#include <SCardSsp.h>
#include <SspsIdl.h>
*/
#include <scclient.h>
#include <scesvc.h>
#include <schannel.h>
#include <Schedule.h>
#include <Schemadef.h>
#include <schnlsp.h>
/*
#include <SCode.h>
*/
#include <ScrnSave.h>
#include <ScrptIDs.h>
#include <scserver.h>
#include <Sddl.h>
#include <sdkddkver.h>
/*
#include <sdoias.h>
*/
#include <sdv_driverspecs.h>
#include <SearchAPI.h>
#include <SecExt.h>
#include <Security.h>
/*
#include <segment.h>
#include <SehMap.h>
*/
#include <Sens.h>
#include <SensAPI.h>
#include <SensEvts.h>
#include <sensors.h>
#include <sensorsapi.h>
#include <ServProv.h>
#include <sessdirpublictypes.h>
#include <SetupAPI.h>
#include <Sfc.h>
#include <shappmgr.h>
#include <shdeprecated.h>
#include <shdispid.h>
#include <ShellAPI.h>
#include <sherrors.h>
/*
//#include <ShFolder.h> //already in shobj.h
#include <shidfact.h>
#include <shimgdata.h>
*/
#include <ShlDisp.h>
#include <ShlGuid.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <ShObjIdl.h>
#include <ShTypes.h>
#include <simpdata.h>
/*
#include <simpdc.h>
*/
#include <SipBase.h>
#include <sisbkup.h>
#include <slerror.h>
#include <sliddefs.h>
#include <slpublic.h>
/*
#include <SMPAB.h>
#include <SMPMS.h>
#include <SMPXP.h>
*/
#include <SmtpGuid.h>
/*
#include <Smx.h>
*/
#include <Snmp.h>
#include <SoftPub.h>
#include <spapidef.h>
/*
#include <spcollec.h>
//#include <spddkhlp.h>
*/
/*
#include <specstrings.h>        //dont add these
#include <specstrings_strict.h>
#include <specstrings_supp.h>
#include <specstrings_undef.h>
*/
#include <sperror.h>
//#include <speventq.h>
//#include <sphelper.h>
//#include <spuihelp.h>
/*
#include <sqltypes.h> //TO REVIEW
#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#include <sqloledb.h>
#include <odbcinst.h>
#include <odbcss.h>
*/
/*
#include <Sql_1.h>
*/
#include <SrRestorePtApi.h>
/*
#include <SspGuid.h>
*/
#include <Sspi.h>
/*
#include <SspsErr.h>
*/
#include <statusdeviceservice.h>
#include <stgprop.h>
#include <Sti.h>
#include <Stierr.h>
#include <Stireg.h>
#include <StlLock.h>
#include <Stm.h>
/*
#include <Storage.h>
*/
#include <storprop.h>
#include <StrAlign.h>
/*
#include <streamcache.h>
#include <streamdescription.h>
*/
#include <strmif.h>
#include <StrongName.h>
#include <strsafe.h>
#include <structuredquery.h>
#include <structuredquerycondition.h>
/*
#include <SubAuth.h>
*/
#include <subscriptionservices.h>
#include <SubsMgr.h>
#include <SvcGuid.h>
/*
#include <SvrAPI.h> //see lmaccess.h
*/
#include <syncdeviceservice.h>
#include <synchronizationerrors.h>
#include <SyncMgr.h>
/*
#include <syncop.h>
*/
#include <syncregistration.h>
#include <syncregistrationerrors.h>
#include <t2embapi.h>
#include <tabflicks.h>
/*
#include <Tapi3.h> //see msp.h
*/
#include <Tapi3cc.h>
#include <Tapi3ds.h>
#include <Tapi3Err.h>
#include <Tapi3if.h>
#include <taskdeviceservice.h>
#include <taskschd.h>
#include <tbs.h>
#include <TCError.h>
#include <TCGuid.h>
#include <tcpestats.h>
#include <tcpioctl.h>
#include <tcpmib.h>
#include <tdh.h>
#include <TermMgr.h>
#include <TextServ.h>
#include <TextStor.h>
#include <ThrdBase.h>
#include <thumbcache.h>
#include <TimeProv.h>
#include <TipAutoComplete.h>
#include <tlbref.h>
#include <TlHelp32.h>
#include <tlogstg.h>
/*
#include <Tmschema.h>
*/
#include <TNEF.h>
#include <TOM.h>
#include <tpcerror.h>
#include <tpcshrd.h>
/*
#include <tpcver.h>
*/
#include <Traffic.h>
#include <Transact.h>
/*
#include <triedcid.h>
#include <triediid.h>
#include <triedit.h>
*/
#include <tsattrs.h>
#include <TSGAuthenticationEngine.h>
#include <TSGPolicyEngine.h>
#include <TSPI.h>
#include <tspubplugincom.h>
#include <tssbx.h>
/*
#include <tsuserex.h>
*/
#include <tsvirtualchannels.h>
/*
#include <tune.h>
#include <tuner.h>
*/
#include <Tvout.h>
/*
#include <tvratings.h>
#include <tvratings_enum.h>
*/
#include <TxCoord.h>
#include <TxDtc.h>
#include <txfw32.h>
#include <txlogpub.h>
/*
#include <UaStrFnc.h>
*/
#include <udpmib.h>
#include <UIAnimation.h>
#include <UIAutomation.h>
#include <UIAutomationClient.h>
#include <UIAutomationCore.h>
#include <UIAutomationCoreApi.h>
/*
#include <UIRibbon.h>
#include <UIRibbonKeydef.h>
#include <UIRibbonPropertyHelpers.h>
#include <Umx.h>
#include <unexposeenums2managed.h>
*/
#include <Unknwn.h>
#include <UPnP.h>
#include <upnphost.h>
#include <UrlHist.h>
#include <UrlMon.h>
#include <UserEnv.h>
#include <usp10.h>
//#include <UtilLib.h>
#include <Uxtheme.h>
#include <vbinterf.h>
/*
#include <Vcr.h>
*/
#include <VdmDbg.h>
#include <vds.h>
#include <vdserr.h>
/*
#include <vdshwprv.h>
*/
#include <vdslun.h>
#include <vdssys.h>
#include <VerError.h>
/*
#include <VerRsrc.h> //dont add
*/
#include <Vfw.h>
#include <vfwmsgs.h>
#include <vidcap.h>
#include <videoacc.h>
#include <virtdisk.h>
#include <vmr9.h>
/*
#include <vpccominterfaces.h>
*/
#include <vptype.h>
#include <vpconfig.h>
#include <vpnotify.h>
#include <vsadmin.h>
/*
#include <vsbackup.h> //only vswriter.h
#include <vswriter.h>
*/
#include <vsmgmt.h>
#include <vsprov.h>
#include <vss.h>
#include <vsserror.h>
#include <vsstyle.h>
#include <vssym32.h>
#include <WbemAds.h>
#include <WbemCli.h>
#include <WbemDisp.h>
//#include <WbemGlue.h>
#include <WbemIdl.h>
#include <WbemProv.h>
//#include <WbemTime.h>
#include <WbemTran.h>
#include <wcmconfig.h>
#include <wcmerrors.h>
#include <WcnApi.h>
#include <WcnDevice.h>
#include <WcnFunctionDiscoveryKeys.h>
#include <WcnTypes.h>
#include <wcsplugin.h>
#include <wct.h>
#include <wdigest.h>
#include <WdsBp.h>
#include <WdsClientApi.h>
#include <wdscpmsg.h>
#include <wdsmcerr.h>
#include <wdspxe.h>
#include <wdstci.h>
#include <wdstpdi.h>
#include <wdstptmgmt.h>
#include <wdstptmgmtmsg.h>
#include <webevnts.h>
//#include <webhost.h>
#include <WebServices.h>
#include <WERAPI.H>
/*
#include <WFExt.h>
*/
#include <wheadef.h>
#include <Wia.h>
#include <WiaDef.h>
#include <WiaDevD.h>
#include <WiaVideo.h>
#include <wiawsdsc.h>
#include <wia_lh.h>
/*
#include <wia_xp.h>
*/
#include <WinBase.h>
#include <Winldap.h> //before winber
#include <WinBer.h>
#include <WinBio_Err.h>
#include <WinBio_Types.h>
#include <WinBio.h> //after types
#include <wincodec.h>
#include <wincodecsdk.h>
#include <WinCon.h>
#include <wincred.h>
#include <WinCrypt.h>
#include <WinDef.h>
#include <WinDNS.h>
#include <windot11.h>
#include <Windows.h>
#include <windowsdefender.h>
#include <windowssearcherrors.h>
#include <WindowsSideShowAPI.h>
#include <WindowsSideShow.h>
//#include <WindowsX.h>
#include <WinEFS.h>
#include <WinError.h>
#include <winevt.h>
#include <WinFax.h>
#include <WinGDI.h>
/*
#include <winhttp.h> //conflicts wininet.h
*/
#include <WinInet.h>
#include <Winineti.h>
#include <winmeta.h>
#include <WinNetWk.h>
#include <WinNls.h>
#include <WinNls32.h>
#include <WinNT.h>
#include <WinPerf.h>
#include <WinReg.h>
#include <WinResrc.h>
#include <winsafer.h>
/*
#include <winsatcominterfacei.h>
#include <WinIoCtl.h>
*/
#include <WinSCard.h> //includes WinIoCtl.h
#include <winsdkver.h>
#include <WinSmCrd.h>
#include <WinSnmp.h>
#include <WinSock.h>
#include <WinSock2.h>
#include <WinSpool.h>
#include <WinSvc.h>
#include <winsxs.h>
#include <winsync.h>
#include <winternl.h>
#include <WinTrust.h>
#include <WinUser.h>
#include <WinVer.h>
#include <WinWlx.h>
#include <wlanapi.h>
#include <wlanihv.h>
#include <wlanihvtypes.h>
#include <wlantypes.h>
#include <wlclient.h>
#include <wmcodecdsp.h>
#include <wmcontainer.h>
#include <wmdm.h>
#include <wmdmlog.h>
#include <wmdrmdeviceapp.h>
#include <wmdrmsdk.h>
#include <wmdxva.h>
#include <Wmistr.h>
#include <WMIUtils.h>
#include <wmlss.h>
#include <wmnetsourcecreator.h>
#include <wmp.h>
#include <wmpdevices.h>
#include <wmpids.h>
#include <wmpplug.h>
#include <wmprealestate.h>
#include <wmpservices.h>
/*
#include <wmsbasicplugin.h>
*/
#include <wmsbuffer.h>
/*
#include <wmscatid.h>
#include <wmscontext.h>
#include <wmsContextNames.h>
#include <wmsdatapath.h>
#include <wmsdefs.h>
*/
#include <wmsdk.h>
#include <wmsdkidl.h>
#include <wmsecure.h>
/*
#include <wmsevent.h>
#include <wmseventlog.h>
#include <wmsheaderline.h>
#include <wmsincomingcounters.h>
*/
#include <wmsinternaladminnetsource.h>
/*
#include <wmsnamedvalues.h>
#include <wmspacket.h>
#include <wmsplaylistparser.h>
#include <wmsplugin.h>
#include <wmsproxy.h>
#include <wmsserver.h>
#include <wmsxmldomextensions.h>
*/
#include <wmsysprf.h>
/*
#include <wnnc.h>
*/
#include <workspaceax.h>
#include <workspaceruntimeclientext.h>
#include <WowNT16.h>
#include <WowNT32.h>
/*
#include <WPApi.h>
#include <WPApiMsg.h>
*/
#include <wpc.h>
#include <wpcapi.h>
#include <wpcevent.h>
/*
#include <WPCrsMsg.h>
*/
#include <wpdmtpextensions.h>
#include <wpdshellextension.h>
#include <wpframework.h>
/*
#include <WPFtpMsg.h>
#include <WPPstMsg.h>
#include <WPSpiHlp.h>
#include <wptypes.h>
#include <WPWizMsg.h>
*/
#include <WS2atm.h>
#include <ws2bth.h>
#include <ws2def.h>
/*
#include <WS2dnet.h>
*/
#include <ws2ipdef.h>
#include <WS2spi.h>
#include <SpOrder.h> //after  WS2spi
#include <WS2tcpip.h>
#include <wsbapp.h>
#include <wsbapperror.h>
#include <Wscapi.h>
#include <wsdapi.h>
#include <wsdattachment.h>
#include <wsdbase.h>
#include <wsdclient.h>
#include <wsddisco.h>
#include <wsdhost.h>
#include <wsdns.h>
#include <wsdtypes.h>
#include <wsdutil.h>
#include <wsdxml.h>
#include <wsdxmldom.h>
#include <WShisotp.h>
#include <wsipv6ok.h>
#include <WSipx.h>
/*
//#include <wsman.h>
#include <wsmandisp.h>
*/
#include <wsmerror.h>
#include <WSnetbs.h>
#include <WSNwLink.h>
#include <WSPiApi.h>
//#include <wsrm.h>
#include <WSvns.h>
#include <WtsApi32.h>
#include <wtsdefs.h>
#include <wtsprotocol.h>
#include <WTypes.h>
#include <wuapi.h>
#include <wuerror.h>
#include <xa.h>
/*
#include <XCMC.h>
#include <XCMCExt.h>
#include <XCMCMSX2.h>
#include <XCMCMSXT.h>
*/
#include <xenroll.h>
#include <Xinput.h>
/*
#include <xmldom.h>
*/
#include <XmlDomDid.h>
#include <XmlDsodid.h>
#include <xmllite.h>
/*
#include <XMLTrnsF.h>
*/
#include <xoleHlp.h>
#include <xpsdigitalsignature.h>
#include <xpsobjectmodel.h>
#include <xpsprint.h>
#include <zmouse.h>
/*
#include <_dbdao.h>
*/	

#undef JET_VERSION
#define JET_VERSION 0x0501

__stdcall DWORD PrvStartDocPrinterW(HANDLE hPrinter, DWORD Level, DOC_INFO_1W *pDocInfo);

HWND WINAPI CreateDialogIndirectParamAorW(HINSTANCE hInstance, LPCDLGTEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit, DWORD Flags);

HWND WINAPI DialogBoxIndirectParamAorW(HINSTANCE hInstance, LPCDLGTEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit, DWORD Flags);
