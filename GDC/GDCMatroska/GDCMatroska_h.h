

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Fri Mar 25 17:58:08 2005
 */
/* Compiler settings for .\GDCMatroska.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __GDCMatroska_h_h__
#define __GDCMatroska_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IIndexMatroska_FWD_DEFINED__
#define __IIndexMatroska_FWD_DEFINED__
typedef interface IIndexMatroska IIndexMatroska;
#endif 	/* __IIndexMatroska_FWD_DEFINED__ */


#ifndef __IndexMatroska_FWD_DEFINED__
#define __IndexMatroska_FWD_DEFINED__

#ifdef __cplusplus
typedef class IndexMatroska IndexMatroska;
#else
typedef struct IndexMatroska IndexMatroska;
#endif /* __cplusplus */

#endif 	/* __IndexMatroska_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IIndexMatroska_INTERFACE_DEFINED__
#define __IIndexMatroska_INTERFACE_DEFINED__

/* interface IIndexMatroska */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IIndexMatroska;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3219E2BC-E7AB-456d-AA17-600E3BDC418D")
    IIndexMatroska : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE HandleFile( 
            BSTR full_path_to_file,
            IDispatch *event_factory) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIndexMatroskaVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIndexMatroska * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIndexMatroska * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIndexMatroska * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIndexMatroska * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIndexMatroska * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIndexMatroska * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIndexMatroska * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *HandleFile )( 
            IIndexMatroska * This,
            BSTR full_path_to_file,
            IDispatch *event_factory);
        
        END_INTERFACE
    } IIndexMatroskaVtbl;

    interface IIndexMatroska
    {
        CONST_VTBL struct IIndexMatroskaVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIndexMatroska_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IIndexMatroska_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IIndexMatroska_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IIndexMatroska_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IIndexMatroska_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IIndexMatroska_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IIndexMatroska_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IIndexMatroska_HandleFile(This,full_path_to_file,event_factory)	\
    (This)->lpVtbl -> HandleFile(This,full_path_to_file,event_factory)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IIndexMatroska_HandleFile_Proxy( 
    IIndexMatroska * This,
    BSTR full_path_to_file,
    IDispatch *event_factory);


void __RPC_STUB IIndexMatroska_HandleFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IIndexMatroska_INTERFACE_DEFINED__ */



#ifndef __GDCMatroskaLib_LIBRARY_DEFINED__
#define __GDCMatroskaLib_LIBRARY_DEFINED__

/* library GDCMatroskaLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_GDCMatroskaLib;

EXTERN_C const CLSID CLSID_IndexMatroska;

#ifdef __cplusplus

class DECLSPEC_UUID("364F941C-6DB0-437d-BBB9-22FCBA88DE30")
IndexMatroska;
#endif
#endif /* __GDCMatroskaLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


