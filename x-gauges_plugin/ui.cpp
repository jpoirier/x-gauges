// Copyright (c) 2011 Joseph D Poirier
// Distributable under the terms of The New BSD License
// that can be found in the LICENSE file.

#if defined(__WINDOWS__)
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

#include "ptypes.h"
#include "pinet.h"

#include "XPLMMenus.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"

#include "nedmalloc.h"
#include "overloaded.h"
#include "defs.h"
#include "plugin.h"
#include "worker_threads.h"
#include "config.h"
#include "ui.h"


void create_widget(int x, int y, int w, int h);
void menu_handler(void* mRef, void* iRef);
void ui_kill(void);
void menu_kill(void);
void ui_hide(void);

USING_PTYPES

#define ISNOT_VISIBLE   0
#define IS_VISIBLE      1

#define ISNOT_ROOT      0
#define IS_ROOT         1

#define UI_X            300
#define UI_Y            650
#define UI_WIDTH        370
#define UI_HEIGHT       250


static int gMenuVisible = false;
static int gMenuWidget = false;

XPLMMenuID gMenuId;
XPWidgetID gSettingsWidget;
XPWidgetID gWidgetWindow;

XPWidgetID gInfoLabel1;
XPWidgetID gInfoLabel2;
XPWidgetID gInfoLabel3;

XPWidgetID gPilot1Label;
XPWidgetID gPilot1Chkbox;
XPWidgetID gPilot1Ip;
XPWidgetID gPilot1Port;

XPWidgetID gPilot2Label;
XPWidgetID gPilot2Chkbox;
XPWidgetID gPilot2Ip;
XPWidgetID gPilot2Port;

XPWidgetID gCopilot1Label;
XPWidgetID gCopilot1Chkbox;
XPWidgetID gCopilot1Ip;
XPWidgetID gCopilot1Port;

XPWidgetID gCopilot2Label;
XPWidgetID gCopilot2Chkbox;
XPWidgetID gCopilot2Ip;
XPWidgetID gCopilot2Port;

XPWidgetID gExitButton;
XPWidgetID gApplyButton;

/*
XPWidgetID settings_receiver_subwindow, settings_sender_subwindow, settings_dest_subwindow;
XPWidgetID dest_ip_textbox[NUM_DEST], dest_port_textbox[NUM_DEST];
XPWidgetID src_port_textbox, nav_rate_textbox, fms_rate_textbox, tcas_rate_textbox;
XPWidgetID version_label, receiver_label, sender_label, dest_label, dest_enable_label, dest_ip_label, dest_port_label, dest_default_label;
XPWidgetID src_port_label, data_rate_label, nav_data_label, fms_data_label, tcas_data_label;
XPWidgetID src_port_default_label, nav_data_default_label, fms_data_default_label, tcas_data_default_label;
XPWidgetID dest_enable_checkbox[NUM_DEST];
XPWidgetID default_local_button, cancel_button, set_button;
*/
void ui_create(void) {

	int item = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "X-Gauges", NULL, true);
	gMenuId = XPLMCreateMenu("X-Gauges", XPLMFindPluginsMenu(), item, menu_handler, NULL);
	XPLMAppendMenuItem(gMenuId, "Network Settings", (void*)"Network Settings", true);
}

void ui_destroy(void) {

    ui_kill();
    menu_kill();
}

void menu_kill(void) {

   	XPLMDestroyMenu(gMenuId);
}

void ui_hide(void) {

	if (gMenuVisible) {

		XPHideWidget(gSettingsWidget);
		gMenuVisible = false;
	}
}

void ui_kill(void) {

	if (gMenuWidget) {

        gMenuWidget = false;
		XPDestroyWidget(gSettingsWidget, true);
	}
}

void menu_handler(void* mRef, void* iRef) {

    if (!strcmp((char*) iRef, "Network Settings")) {
		if (!gMenuWidget) {

            gMenuWidget = true;
            create_widget(UI_X, UI_Y, UI_WIDTH, UI_HEIGHT);
		} else if (!XPIsWidgetVisible(gSettingsWidget)) {

            XPShowWidget(gSettingsWidget);
		}

		gMenuVisible = true;
    }
}

// define the handler before it is used
int callback_handler(XPWidgetMessage    inMessage,
                     XPWidgetID         inWidget,
                     long               inParam1,
                     long               inParam2) {

	char buffer[24];

	// we are not displaying the close (X) to avoid uncertainty about its logic (is it Cancel or OK?)
	if (inMessage == xpMessage_CloseButtonPushed) {

		ui_hide();
		return true;
	}

	if (inMessage == xpMsg_PushButtonPressed) {

		if (inParam1 == (long)gExitButton) {

            ui_hide();
            return true;
        }

        if (inParam1 == (long)gApplyButton) {
            GaugeInfo* s;
            bool reconfig;
            string tmp;

            // pilot 1
            reconfig = false;
            memset(buffer, 0, sizeof(buffer));
            XPGetWidgetDescriptor(gPilot1Ip, buffer, sizeof(buffer));
            tmp = string(buffer);

            if (gP1_ip != tmp) {

                gP1_ip = tmp;
                reconfig = true;
            }

            memset(buffer, 0, sizeof(buffer));
            XPGetWidgetDescriptor(gPilot1Port, buffer, sizeof(buffer));
            tmp = string(buffer);

            if (gP1_port != tmp) {

                gP1_port = tmp;
                reconfig = true;
            }

            if (reconfig)
                p1->net_config(gP1_ip, gP1_port);

            tmp = itostring((int)XPGetWidgetProperty(gPilot1Chkbox, xpProperty_ButtonState, NULL));

            if (gP1_enabled != tmp) {

                gP1_enabled = tmp;

                if (tmp == '1') {
                    gP1Trigger.post();
                } else {
                    gP1Trigger.reset();
                    s = (GaugeInfo*) malloc(sizeof(GaugeInfo));
                    s->sys_magic = false;
                    gP1_ijq.post(new myjob(s));
                }
            }

            // pilot 2
            reconfig = false;
            memset(buffer, 0, sizeof(buffer));
            XPGetWidgetDescriptor(gPilot2Ip, buffer, sizeof(buffer));
            tmp = string(buffer);

            if (gP2_ip != tmp) {

                gP2_ip = tmp;
                reconfig = true;
            }

            memset(buffer, 0, sizeof(buffer));
            XPGetWidgetDescriptor(gPilot2Port, buffer, sizeof(buffer));
            tmp = string(buffer);

            if (gP2_port != tmp) {

                gP2_port = tmp;
                reconfig = true;
            }

            if (reconfig)
                p2->net_config(gP2_ip, gP2_port);

            tmp = itostring((int)XPGetWidgetProperty(gPilot2Chkbox, xpProperty_ButtonState, NULL));

            if (gP2_enabled != tmp) {

                gP2_enabled = tmp;

                if (tmp == '1') {
                    gP2Trigger.post();
                } else  {
                    gP2Trigger.reset();
                    s = (GaugeInfo*) malloc(sizeof(GaugeInfo));
                    s->sys_magic = false;
                    gP2_ijq.post(new myjob(s));
                }
            }

            // copilot 1
            reconfig = false;
            memset(buffer, 0, sizeof(buffer));
            XPGetWidgetDescriptor(gCopilot1Ip, buffer, sizeof(buffer));
            tmp = string(buffer);

            if (gCp1_ip != tmp) {

                gCp1_ip = tmp;
                reconfig = true;
            }

            memset(buffer, 0, sizeof(buffer));
            XPGetWidgetDescriptor(gCopilot1Port, buffer, sizeof(buffer));
            tmp = string(buffer);

            if (gCp1_port != tmp) {

                gCp1_port = tmp;
                reconfig = true;
            }

            if (reconfig)
                cp1->net_config(gCp1_ip, gCp1_port);

            tmp = itostring((int)XPGetWidgetProperty(gCopilot1Chkbox, xpProperty_ButtonState, NULL));

            if (gCp1_enabled != tmp) {

                gCp1_enabled = tmp;

                if (tmp == '1') {
                    gCp1Trigger.post();
                } else  {
                    gCp1Trigger.reset();
                    s = (GaugeInfo*) malloc(sizeof(GaugeInfo));
                    s->sys_magic = false;
                    gCp1_ijq.post(new myjob(s));
                }
            }

            // copilot 2
            memset(buffer, 0, sizeof(buffer));
            XPGetWidgetDescriptor(gCopilot2Ip, buffer, sizeof(buffer));
            tmp = string(buffer);

            if (gCp2_ip != tmp) {

                gCp2_ip = tmp;
                reconfig = true;
            }

            memset(buffer, 0, sizeof(buffer));
            XPGetWidgetDescriptor(gCopilot2Port, buffer, sizeof(buffer));
            tmp = string(buffer);

            if (gCp2_port != tmp) {

                gCp2_port = tmp;
                reconfig = true;
            }

            if (reconfig)
                cp2->net_config(gCp2_ip, gCp2_port);

            tmp = itostring((int) XPGetWidgetProperty(gCopilot2Chkbox, xpProperty_ButtonState, NULL));

            if (gCp2_enabled != tmp) {

                gCp2_enabled = tmp;

                if (tmp == '1') {
                    gCp2Trigger.post();
                } else  {
                    gCp2Trigger.reset();
                    s = (GaugeInfo*) malloc(sizeof(GaugeInfo));
                    s->sys_magic = false;
                    gCp2_ijq.post(new myjob(s));
                }
            }
        }
	}

	return false;
}


void create_widget(int x, int y, int w, int h) {

    int x2 = x + w;
    int y2 = y - h;

	// root window
	gSettingsWidget = XPCreateWidget(x, y, x2, y2, IS_VISIBLE, "X-Gauges Network Settings", IS_ROOT, NULL, xpWidgetClass_MainWindow);
	XPSetWidgetProperty(gSettingsWidget, xpProperty_MainWindowHasCloseBoxes, true);

    // container
	gWidgetWindow = XPCreateWidget(x+10, y-30, x2-10, y2+10, IS_VISIBLE, "", ISNOT_ROOT, gSettingsWidget, xpWidgetClass_SubWindow);
	XPSetWidgetProperty(gWidgetWindow, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);

    // information labels
	gInfoLabel1 = XPCreateWidget(x+90, y-40, x+150, y-60, IS_VISIBLE, "IP ADDRESS", ISNOT_ROOT, gSettingsWidget, xpWidgetClass_Caption);
	gInfoLabel2 = XPCreateWidget(x+220, y-40, x+280, y-60, IS_VISIBLE, "PORT", ISNOT_ROOT, gSettingsWidget, xpWidgetClass_Caption);
	gInfoLabel3 = XPCreateWidget(x+300, y-40, x+370, y-60, IS_VISIBLE, "ENABLE", ISNOT_ROOT, gSettingsWidget, xpWidgetClass_Caption);

    // Pilot 1
	gPilot1Label = XPCreateWidget(x+20, y-60, x+70, y-80, IS_VISIBLE, "PILOT   1: ", ISNOT_ROOT, gSettingsWidget, xpWidgetClass_Caption);
    gPilot1Ip = XPCreateWidget(x+90, y-60, x+200, y-80, IS_VISIBLE, gP1_ip, ISNOT_ROOT, gSettingsWidget, xpWidgetClass_TextField);
    XPSetWidgetProperty(gPilot1Ip, xpProperty_TextFieldType, xpTextEntryField);
    XPSetWidgetProperty(gPilot1Ip, xpProperty_Enabled, true);

    gPilot1Port = XPCreateWidget(x+220, y-60, x+280, y-80, IS_VISIBLE, gP1_port, ISNOT_ROOT, gSettingsWidget, xpWidgetClass_TextField);
    XPSetWidgetProperty(gPilot1Port, xpProperty_TextFieldType, xpTextEntryField);
    XPSetWidgetProperty(gPilot1Port, xpProperty_Enabled, true);

    gPilot1Chkbox = XPCreateWidget(x+300, y-60, x+340, y-80, IS_VISIBLE, "", ISNOT_ROOT, gSettingsWidget, xpWidgetClass_Button);
    XPSetWidgetProperty(gPilot1Chkbox, xpProperty_ButtonType, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(gPilot1Chkbox, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(gPilot1Chkbox, xpProperty_ButtonState, (long)stringtoi(gP1_enabled));

    // Pilot 2
	gPilot2Label = XPCreateWidget(x+20, y-80, x+70, y-100, IS_VISIBLE, "PILOT   2: ", ISNOT_ROOT, gSettingsWidget, xpWidgetClass_Caption);
    gPilot2Ip = XPCreateWidget(x+90, y-80, x+200, y-100, IS_VISIBLE, gP2_ip, ISNOT_ROOT, gSettingsWidget, xpWidgetClass_TextField);
    XPSetWidgetProperty(gPilot2Ip, xpProperty_TextFieldType, xpTextEntryField);
    XPSetWidgetProperty(gPilot2Ip, xpProperty_Enabled, true);

    gPilot2Port = XPCreateWidget(x+220, y-80, x+280, y-100, IS_VISIBLE, gP2_port, ISNOT_ROOT, gSettingsWidget, xpWidgetClass_TextField);
    XPSetWidgetProperty(gPilot2Port, xpProperty_TextFieldType, xpTextEntryField);
    XPSetWidgetProperty(gPilot2Port, xpProperty_Enabled, true);

    gPilot2Chkbox = XPCreateWidget(x+300, y-80, x+340, y-100, IS_VISIBLE, "", ISNOT_ROOT, gSettingsWidget, xpWidgetClass_Button);
    XPSetWidgetProperty(gPilot2Chkbox, xpProperty_ButtonType, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(gPilot2Chkbox, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(gPilot2Chkbox, xpProperty_ButtonState, (long)stringtoi(gP2_enabled));

    // CoPilot 1
	gCopilot1Label = XPCreateWidget(x+20, y-110, x+70, y-130, IS_VISIBLE, "COPILOT 1: ", ISNOT_ROOT, gSettingsWidget, xpWidgetClass_Caption);
    gCopilot1Ip = XPCreateWidget(x+90, y-110, x+200, y-130, IS_VISIBLE, gCp1_ip, ISNOT_ROOT, gSettingsWidget, xpWidgetClass_TextField);
    XPSetWidgetProperty(gCopilot1Ip, xpProperty_TextFieldType, xpTextEntryField);
    XPSetWidgetProperty(gCopilot1Ip, xpProperty_Enabled, true);

    gCopilot1Port = XPCreateWidget(x+220, y-110, x+280, y-130, IS_VISIBLE, gCp1_port, ISNOT_ROOT, gSettingsWidget, xpWidgetClass_TextField);
    XPSetWidgetProperty(gCopilot1Port, xpProperty_TextFieldType, xpTextEntryField);
    XPSetWidgetProperty(gCopilot1Port, xpProperty_Enabled, true);

    gCopilot1Chkbox = XPCreateWidget(x+300, y-110, x+340, y-130, IS_VISIBLE, "", ISNOT_ROOT, gSettingsWidget, xpWidgetClass_Button);
    XPSetWidgetProperty(gCopilot1Chkbox, xpProperty_ButtonType, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(gCopilot1Chkbox, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(gCopilot1Chkbox, xpProperty_ButtonState, (long)stringtoi(gCp1_enabled));

    // CoPilot 2
	gCopilot2Label = XPCreateWidget(x+20, y-130, x+70, y-150, IS_VISIBLE, "COPILOT 2: ", ISNOT_ROOT, gSettingsWidget, xpWidgetClass_Caption);
    gCopilot2Ip = XPCreateWidget(x+90, y-130, x+200, y-150, IS_VISIBLE, gCp2_ip, ISNOT_ROOT, gSettingsWidget, xpWidgetClass_TextField);
    XPSetWidgetProperty(gCopilot2Ip, xpProperty_TextFieldType, xpTextEntryField);
    XPSetWidgetProperty(gCopilot2Ip, xpProperty_Enabled, true);

    gCopilot2Port = XPCreateWidget(x+220, y-130, x+280, y-150, IS_VISIBLE, gCp2_port, ISNOT_ROOT, gSettingsWidget, xpWidgetClass_TextField);
    XPSetWidgetProperty(gCopilot2Port, xpProperty_TextFieldType, xpTextEntryField);
    XPSetWidgetProperty(gCopilot2Port, xpProperty_Enabled, true);

    gCopilot2Chkbox = XPCreateWidget(x+300, y-130, x+340, y-150, IS_VISIBLE, "", ISNOT_ROOT, gSettingsWidget, xpWidgetClass_Button);
    XPSetWidgetProperty(gCopilot2Chkbox, xpProperty_ButtonType, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(gCopilot2Chkbox, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(gCopilot2Chkbox, xpProperty_ButtonState, (long)stringtoi(gCp2_enabled));

	// Apply and Exit buttons
	gApplyButton = XPCreateWidget(x+90, y-210, x+150, y-230, IS_VISIBLE, "Apply", ISNOT_ROOT, gSettingsWidget, xpWidgetClass_Button);
	XPSetWidgetProperty(gApplyButton, xpProperty_ButtonType, xpPushButton);

	gExitButton = XPCreateWidget(x+220, y-210, x+280, y-230, IS_VISIBLE, "Exit", ISNOT_ROOT, gSettingsWidget, xpWidgetClass_Button);
	XPSetWidgetProperty(gExitButton, xpProperty_ButtonType, xpPushButton);

	XPAddWidgetCallback(gSettingsWidget, callback_handler);
}

