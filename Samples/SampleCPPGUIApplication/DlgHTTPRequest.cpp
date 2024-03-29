// Author: Mohammad D. Mottaghi

/* Copyright (c) 2015, NVIDIA CORPORATION. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the name of NVIDIA CORPORATION nor the names of its
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.
 
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS "AS IS" AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "afxdialogex.h"
#include <string>
#include <Winhttp.h>
#include"DlgHTTPRequest.h"
#include"HttpClient.h"
#include "GFNLinkSDK_CAPI.hpp"
#include "base64.h"

using namespace GFNLinkSDK;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDM_SIMULATOR_PATH L"http://localhost:55443/idmSimulator"

using namespace std;

BEGIN_MESSAGE_MAP(CHTTPRequestDlg, CDialogEx)	
	ON_BN_CLICKED(rdCustomIDM, &CHTTPRequestDlg::CustomIDMClick)
	ON_BN_CLICKED(rdSimIDM, &CHTTPRequestDlg::RestoreDefaults)
	ON_BN_CLICKED(btnResolveSymbol, &CHTTPRequestDlg::btnResolveSymbolClick)
	ON_BN_CLICKED(btnProfile, &CHTTPRequestDlg::btnProfileClick)
	ON_BN_CLICKED(btnGo, &CHTTPRequestDlg::btnGoClick)
	ON_BN_CLICKED(btnClear, &CHTTPRequestDlg::btnClearClick)
END_MESSAGE_MAP()
//------------------------------------------------------------------------------------------------------------------------
CHTTPRequestDlg::CHTTPRequestDlg() : CDialogEx(CHTTPRequestDlg::IDD)
{
}
//------------------------------------------------------------------------------------------------------------------------
void CHTTPRequestDlg::ShowResponse(const char *rsp, size_t len)
{
	const size_t cSize = len + 1;
	wchar_t* w_resp = new wchar_t[cSize];
	mbstowcs(w_resp, rsp, cSize);
	txtResponse.SetWindowText(w_resp);
	delete[]w_resp;
}
//------------------------------------------------------------------------------------------------------------------------
void CHTTPRequestDlg::ReportError(const char *formatted_msg, int err_code)
{
	const size_t buf_len = 1000;
	char msg[buf_len] = "Failed ! ";
	if (err_code > 0)
		sprintf(msg + strlen(msg), formatted_msg, formatted_msg);
	else
		strcpy(msg + strlen(msg), formatted_msg);
	ShowResponse(msg, strlen(msg));
}
//------------------------------------------------------------------------------------------------------------------------
void CHTTPRequestDlg::btnClearClick()
{
	txtResponse.SetWindowText(L"");
}
//------------------------------------------------------------------------------------------------------------------------
void CHTTPRequestDlg::CustomIDMClick()
{
	GetDlgItem(btnProfile)->EnableWindow(false);
	GetDlgItem(btnGo)->SetWindowText(L"Pos&t Data");
	SetWindowText(L"HTTP-Post Composer");
	txtUrl.SetWindowText(L"http");
	txtHeaders.SetWindowText(L"");
}
//------------------------------------------------------------------------------------------------------------------------
void CHTTPRequestDlg::RestoreDefaults()
{
	GetDlgItem(btnGo)->SetWindowText(L"Get Session &Token");
	GetDlgItem(btnProfile)->EnableWindow(true);
	((CButton*)this->GetDlgItem(rdSimIDM))->SetCheck(true);
	SetWindowText(L"3rd-Party-IDM Simulator");
	
	txtUrl.SetWindowText(IDM_SIMULATOR_PATH);
	txtHeaders.SetWindowText(TEXT("Authorization: Basic @b64(<Token>:)"));
	txtBody.SetWindowText(TEXT("")); // 
//	txtUrl.SetWindowText(TEXT("http://nvslb01.nvidia.com/api/1/authentication/user/login"));  
//	txtBody.SetWindowText(TEXT("{\"deviceId\": \"94211609774260224\"}")); // "token=<Token>"
//	txtHeaders.SetWindowText(TEXT("Authorization: Basic dGVzdEBnbWFpbC5jb206MTIzNEhlbGxv\r\ncontent-type:application/json"));
}
//------------------------------------------------------------------------------------------------------------------------
void CHTTPRequestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, txtBody);
	DDX_Control(pDX, IDC_EDIT2, txtUrl);
	DDX_Control(pDX, IDC_EDIT3, txtHeaders);
	DDX_Control(pDX, IDC_EDIT4, txtResponse);
	RestoreDefaults();
}
//------------------------------------------------------------------------------------------------------------------------
void CHTTPRequestDlg::ResolveSymbols(CString *s)
{
	wstring ws = s->GetBuffer();
	string ns(ws.begin(), ws.end());

	if (idm_simulator.SessionToken()[0] == 0) // empty token
	{
		const char *delegate_token = "";
		GFNLinkError err = GFNLinkSDK::Instance()->RequestGFNAccessToken(&delegate_token);
		idm_simulator.ObtainSessionToken(delegate_token);
	}

	// replace <token> and <Token> with the real token
	for (size_t tkn_pos = 0; (tkn_pos = ns.find("<token>", tkn_pos)) != string::npos;)
		ns.replace(tkn_pos, 7 /*strlen("<token>")*/, idm_simulator.SessionToken());
	for (size_t tkn_pos = 0; (tkn_pos = ns.find("<Token>", tkn_pos)) != string::npos;)
		ns.replace(tkn_pos, 7 /*strlen("<token>")*/, idm_simulator.SessionToken());

	// apply the Base64 conversion ( ie, @b64(...) )
	for (size_t bpos = 0; (bpos = ns.find("@b64(", bpos)) != string::npos;)
	{
		long opened = 1, inp_len = 0;
		for (const char *chr = ns.c_str() + bpos + 5; opened > 0 && *chr != 0; chr++, inp_len++)
			if (*chr == '(')
				opened++;
			else if (*chr == ')')
				opened--;
					
		string base64 = base64_encode((const unsigned char *)ns.c_str()+bpos+5, inp_len-1);
		ns.replace(bpos, inp_len + 5, base64);
	}

	ws.assign(ns.begin(), ns.end());
	*s = ws.c_str();
}
//------------------------------------------------------------------------------------------------------------------------
void CHTTPRequestDlg::btnGoClick()
{
	CString given_url;
	txtUrl.GetWindowText(given_url);

	if (given_url.Compare(IDM_SIMULATOR_PATH) == 0)
	{
		string success_results = "";
		const char *simulated_resp, *delegate_token = "";
		GFNLinkError err = GFNLinkSDK::Instance()->RequestGFNAccessToken(&delegate_token);
		if (idm_simulator.ObtainSessionToken(delegate_token))
			simulated_resp = idm_simulator.ErrorMessage();
		else
		{
			success_results = "Session token received successfully:\r\n";
			success_results += idm_simulator.SessionToken();
			simulated_resp = success_results.c_str();
		}

		ShowResponse(simulated_resp, strlen(simulated_resp));
	}
	else
	{
		CString req_body, req_header;
		txtBody.GetWindowText(req_body);
		txtHeaders.GetWindowText(req_header);

		ResolveSymbols(&req_body);
		ResolveSymbols(&req_header);

		CHttpClient http(L"NVidia Sample Game");
		const char *real_idm_resp = http.Post(given_url.GetBuffer(), req_header.GetBuffer(), req_body.GetBuffer());
		ShowResponse(real_idm_resp, strlen(real_idm_resp));
	}
}
//------------------------------------------------------------------------------------------------------------------------
void CHTTPRequestDlg::btnResolveSymbolClick()
{
	CString bdy, hdr;
	txtBody.GetWindowText(bdy);
	txtHeaders.GetWindowText(hdr);

	ResolveSymbols(&bdy);
	ResolveSymbols(&hdr);
	
	txtBody.SetWindowText(bdy);
	txtHeaders.SetWindowText(hdr);
}
//------------------------------------------------------------------------------------------------------------------------
void CHTTPRequestDlg::btnProfileClick()
{
	const char *response;
	string user_profile = idm_simulator.GetUserProfile();
	if (user_profile.length() == 0)
		response = idm_simulator.GetLastError() == iseInvalidSessionToken ? "Error: Get a session token first." : idm_simulator.ErrorMessage();
	else
		response = user_profile.c_str();

	ShowResponse(response, strlen(response));
}
//------------------------------------------------------------------------------------------------------------------------
