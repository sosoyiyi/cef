// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/cefclient/renderer/performance_test.h"

#include <algorithm>
#include <string>
#include <iostream>
#include "include/base/cef_logging.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include "tests/cefclient/renderer/performance_test_setup.h"
#include "tests/shared/common/client_utils.h"

namespace client {
	namespace performance_test {

		// Use more interations for a Release build.
#if DCHECK_IS_ON()
		const int kDefaultIterations = 100000;
#else
		const int kDefaultIterations = 10000;
#endif

		namespace {

			//const char kGetPerfTests[] = "GetPerfTests";
			//const char kRunPerfTest[] = "RunPerfTest";
			//const char kPerfTestReturnValue[] = "PerfTestReturnValue";
			const char kExecute[] = "Execute";
			const std::string initCode = "var qrcode = {"
				"create : function(e, w, h) {"
				"e.innerHTML = '';"
				"var f = new QRCode(e, {"
				"width: w,"
				"height : h,"
				"colorDark : '#000000',"
				"colorLight : '#ffffff',"
				"correctLevel : QRCode.CorrectLevel.L"
				"});"
				"return f;"
				"}"
				"}";
			const std::string makeCode =
				"(function(){"
				"$(\"table:eq(0) td:eq(0)\").append(\"<div id='qrcode'></div>\");"
				"$(\"table:eq(1) td:eq(3)\").append(\"<div id='qrcode-ios'></div>\");"
				"$(\"table:eq(1) td:eq(4)\").append(\"<div id='qrcode-android'></div>\");"
				"qrcode.create(document.getElementById('qrcode'), 256, 256).makeCode('123');"
				"qrcode.create(document.getElementById('qrcode-android'), 128, 128).makeCode('http://www.baidu.com');"
				"qrcode.create(document.getElementById('qrcode-ios'), 128, 128).makeCode('http://www.qq.com');"
				"})();";
			std::string uniqueCode="";
			class V8Handler : public CefV8Handler {
			public:
				V8Handler() {}
				virtual bool Execute(const CefString& name,
					CefRefPtr<CefV8Value> object,
					const CefV8ValueList& arguments,
					CefRefPtr<CefV8Value>& retval,
					CefString& exception) OVERRIDE {
					CefRefPtr<CefFrame> frame = CefV8Context::GetCurrentContext()->GetBrowser()->GetMainFrame();
					if (uniqueCode.empty()) {
						std::vector<std::string> urlParam = ClientUtils::split(frame->GetURL(), "=");
						if (urlParam.size() <= 1) {
							uniqueCode = "NULL";
						}
						else {
							uniqueCode = urlParam[1];
						}
					}
					if (name == kExecute) {
						if (arguments.size() == 1 && arguments[0]->IsString()) {
							if (arguments.at(0)->GetStringValue() == "MAKECODE") {
								frame->ExecuteJavaScript(initCode, frame->GetURL(), 0);
								frame->ExecuteJavaScript(makeCode, frame->GetURL(), 0);
							}
							else if(arguments.at(0)->GetStringValue()=="GENSN") {
								retval= CefV8Value::CreateString(uniqueCode);
							}
						}
						else if (arguments.size() == 2 && arguments[0]->IsString()) {
							if (arguments.at(0)->GetStringValue() == "MAKECODE" && arguments[1]->IsString()) {
								frame->ExecuteJavaScript(initCode, frame->GetURL(), 0);
								frame->ExecuteJavaScript(
									"qrcode.create(document.getElementById('qrcode'), 256, 256).makeCode('" + arguments.at(1)->GetStringValue().ToString() + "');"
									, frame->GetURL(), 0);
							}
						}
					}
					/*if (name == kRunPerfTest) {
					  if (arguments.size() == 1 && arguments[0]->IsString()) {
						// Run the specified perf test.
						bool found = false;

						std::string test = arguments[0]->GetStringValue();
						for (int i = 0; i < kPerfTestsCount; ++i) {
						  if (test == kPerfTests[i].name) {
							// Execute the test.
							int64 delta = kPerfTests[i].test(kPerfTests[i].iterations);

							retval = CefV8Value::CreateInt(delta);
							found = true;
							break;
						  }
						}

						if (!found) {
						  std::string msg = "Unknown test: ";
						  msg.append(test);
						  exception = msg;
						}
					  } else {
						exception = "Invalid function parameters";
					  }
					} else if (name == kGetPerfTests) {
					  // Retrieve the list of perf tests.
					  retval = CefV8Value::CreateArray(kPerfTestsCount);
					  for (int i = 0; i < kPerfTestsCount; ++i) {
						CefRefPtr<CefV8Value> val = CefV8Value::CreateArray(2);
						val->SetValue(0, CefV8Value::CreateString(kPerfTests[i].name));
						val->SetValue(1, CefV8Value::CreateUInt(kPerfTests[i].iterations));
						retval->SetValue(i, val);
					  }
					} else if (name == kPerfTestReturnValue) {
						if (arguments.size() == 0) {
							retval = CefV8Value::CreateInt(1);
						}
						else if (arguments.size() == 1 && arguments[0]->IsInt()) {
							int32 type = arguments[0]->GetIntValue();
							CefTime date;
							switch (type) {
							case 0:
								retval = CefV8Value::CreateUndefined();
								break;
							case 1:
								retval = CefV8Value::CreateNull();
								break;
							case 2:
								retval = CefV8Value::CreateBool(true);
								break;
							case 3:
								retval = CefV8Value::CreateInt(1);
								break;
							case 4:
								retval = CefV8Value::CreateUInt(1);
								break;
							case 5:
								retval = CefV8Value::CreateDouble(1.234);
								break;
							case 6:
								date.Now();
								retval = CefV8Value::CreateDate(date);
								break;
							case 7:
								retval = CefV8Value::CreateString("Hello, world!");
								break;
							case 8:
								retval = CefV8Value::CreateObject(NULL, NULL);
								break;
							case 9:
								retval = CefV8Value::CreateArray(8);
								break;
							case 10:
								// retval = CefV8Value::CreateFunction(...);
								exception = "Not implemented";
								break;
							default:
								exception = "Not supported";
							}
						}
					}*/

					return true;
				}

			private:
				IMPLEMENT_REFCOUNTING(V8Handler);
			};

			// Handle bindings in the render process.
			class RenderDelegate : public ClientAppRenderer::Delegate {
			public:
				RenderDelegate() {}

				virtual void OnContextCreated(CefRefPtr<ClientAppRenderer> app,
					CefRefPtr<CefBrowser> browser,
					CefRefPtr<CefFrame> frame,
					CefRefPtr<CefV8Context> context) OVERRIDE {
					CefRefPtr<CefV8Value> object = context->GetGlobal();

					CefRefPtr<CefV8Handler> handler = new V8Handler();
					object->SetValue(kExecute, CefV8Value::CreateFunction(kExecute, handler),
						V8_PROPERTY_ATTRIBUTE_READONLY);
					// Bind functions.
					/*object->SetValue(kGetPerfTests,
						CefV8Value::CreateFunction(kGetPerfTests, handler),
						V8_PROPERTY_ATTRIBUTE_READONLY);
					object->SetValue(kRunPerfTest,
						CefV8Value::CreateFunction(kRunPerfTest, handler),
						V8_PROPERTY_ATTRIBUTE_READONLY);
					object->SetValue(kPerfTestReturnValue,
						CefV8Value::CreateFunction(kPerfTestReturnValue, handler),
						V8_PROPERTY_ATTRIBUTE_READONLY);
						*/
				}

			private:
				IMPLEMENT_REFCOUNTING(RenderDelegate);
			};

		}  // namespace

		void CreateDelegates(ClientAppRenderer::DelegateSet& delegates) {
			delegates.insert(new RenderDelegate);
		}

	}  // namespace performance_test
}  // namespace client
