/**
 * @file winmenu.h
 * @author Joe Wingbermuehle
 * @date 2004-2006
 *
 * @brief Functions for handling window menus.
 *
*/

#include "jwm.h"
#include "winmenu.h"
#include "client.h"
#include "menu.h"
#include "main.h"
#include "desktop.h"
#include "move.h"
#include "resize.h"
#include "event.h"
#include "cursor.h"
#include "misc.h"
#include "root.h"

static Menu *CreateWindowMenu();
static void RunWindowCommand(const MenuAction *action);

static void CreateWindowLayerMenu(Menu *menu);
static void CreateWindowSendToMenu(Menu *menu);
static void AddWindowMenuItem(Menu *menu, const char *name,
                              MenuActionType type, int value);

static ClientNode *client = NULL;

/** Get the size of a window menu. */
void GetWindowMenuSize(ClientNode *np, int *width, int *height) {

   Menu *menu;

   client = np;
   menu = CreateWindowMenu();
   InitializeMenu(menu);
   *width = menu->width;
   *height = menu->height;
   DestroyMenu(menu);

}

/** Show a window menu. */
void ShowWindowMenu(ClientNode *np, int x, int y) {

   Menu *menu;

   client = np;
   menu = CreateWindowMenu();

   InitializeMenu(menu);

   ShowMenu(menu, RunWindowCommand, x, y);

   DestroyMenu(menu);

}

/** Create a new window menu. */
Menu *CreateWindowMenu() {

   Menu *menu;

   menu = Allocate(sizeof(Menu));
   menu->itemHeight = 0;
   menu->items = NULL;
   menu->label = NULL;

   /* Note that items are added in reverse order of display. */

   if(!(client->state.status & STAT_WMDIALOG)) {
      AddWindowMenuItem(menu, _("Close"), MA_CLOSE, 0);
      AddWindowMenuItem(menu, _("Kill"), MA_KILL, 0);
      AddWindowMenuItem(menu, NULL, MA_NONE, 0);
   }

   if(client->state.status & (STAT_MAPPED | STAT_SHADED)) {
      if(client->state.border & BORDER_RESIZE) {
         AddWindowMenuItem(menu, _("Resize"), MA_RESIZE, 0);
      }
      if(client->state.border & BORDER_MOVE) {
         AddWindowMenuItem(menu, _("Move"), MA_MOVE, 0);
      }
   }

   if(client->state.border & BORDER_MIN) {

      if(client->state.status & STAT_MINIMIZED) {
         AddWindowMenuItem(menu, _("Restore"), MA_RESTORE, 0);
      } else {
         if(client->state.status & STAT_SHADED) {
            AddWindowMenuItem(menu, _("Unshade"), MA_SHADE, 0);
         } else {
            AddWindowMenuItem(menu, _("Shade"), MA_SHADE, 0);
         }
         AddWindowMenuItem(menu, _("Minimize"), MA_MINIMIZE, 0);
      }

   }

   if((client->state.border & BORDER_MAX)
      && (client->state.status & STAT_MAPPED)) {

      if(!(client->state.status & (STAT_HMAX | STAT_VMAX))) {
         AddWindowMenuItem(menu, _("Maximize-y"), MA_MAXIMIZE_V, 0);
      }

      if(!(client->state.status & (STAT_HMAX | STAT_VMAX))) {
         AddWindowMenuItem(menu, _("Maximize-x"), MA_MAXIMIZE_H, 0);
      }

      if((client->state.status & (STAT_HMAX | STAT_VMAX))) {
         AddWindowMenuItem(menu, _("Restore"), MA_MAXIMIZE, 0);
      } else {
         AddWindowMenuItem(menu, _("Maximize"), MA_MAXIMIZE, 0);
      }

   }

   if(!(client->state.status & STAT_WMDIALOG)) {

      if(client->state.status & STAT_STICKY) {
         AddWindowMenuItem(menu, _("Unstick"), MA_STICK, 0);
      } else {
         AddWindowMenuItem(menu, _("Stick"), MA_STICK, 0);
      }

      CreateWindowLayerMenu(menu);

      if(!(client->state.status & STAT_STICKY)) {
         CreateWindowSendToMenu(menu);
      }

   }

   return menu;
}

/** Create a window layer submenu. */
void CreateWindowLayerMenu(Menu *menu) {

   Menu *submenu;
   MenuItem *item;
   char str[10];
   unsigned int x;

   item = Allocate(sizeof(MenuItem));
   item->type = MENU_ITEM_SUBMENU;
   item->name = CopyString(_("Layer"));
   item->action.type = MA_NONE;
   item->action.data.str = NULL;
   item->iconName = NULL;

   item->next = menu->items;
   menu->items = item;

   submenu = Allocate(sizeof(Menu));
   item->submenu = submenu;
   submenu->itemHeight = 0;
   submenu->items = NULL;
   submenu->label = NULL;

   if(client->state.layer == LAYER_TOP) {
      AddWindowMenuItem(submenu, _("[Top]"), MA_LAYER, LAYER_TOP);
   } else {
      AddWindowMenuItem(submenu, _("Top"), MA_LAYER, LAYER_TOP);
   }

   str[4] = 0;
   for(x = LAYER_TOP - 1; x > LAYER_BOTTOM; x--) {
      if(x == LAYER_NORMAL) {
         if(client->state.layer == x) {
            AddWindowMenuItem(submenu, _("[Normal]"), MA_LAYER, x);
         } else {
            AddWindowMenuItem(submenu, _("Normal"), MA_LAYER, x);
         }
      } else {
         if(client->state.layer == x) {
            str[0] = '[';
            str[3] = ']';
         } else {
            str[0] = ' ';
            str[3] = ' ';
         }
         if(x < 10) {
            str[1] = ' ';
         } else {
            str[1] = (x / 10) + '0';
         }
         str[2] = (x % 10) + '0';
         AddWindowMenuItem(submenu, str, MA_LAYER, x);
      }
   }

   if(client->state.layer == LAYER_BOTTOM) {
      AddWindowMenuItem(submenu, _("[Bottom]"), MA_LAYER, LAYER_BOTTOM);
   } else {
      AddWindowMenuItem(submenu, _("Bottom"), MA_LAYER, LAYER_BOTTOM);
   }

}

/** Create a send to submenu. */
void CreateWindowSendToMenu(Menu *menu) {

   unsigned int mask;
   unsigned int x;

   mask = 0;
   for(x = 0; x < desktopCount; x++) {
      if(client->state.desktop == x
         || (client->state.status & STAT_STICKY)) {
         mask |= 1 << x;
      }
   }

   AddWindowMenuItem(menu, _("Send To"), MA_NONE, 0);

   /* Now the first item in the menu is for the desktop list. */
   menu->items->submenu = CreateDesktopMenu(mask);

}

/** Add an item to the current window menu. */
void AddWindowMenuItem(Menu *menu, const char *name,
                       MenuActionType type, int value) {

   MenuItem *item;

   item = Allocate(sizeof(MenuItem));
   if(name) {
      item->type = MENU_ITEM_NORMAL;
   } else {
      item->type = MENU_ITEM_SEPARATOR;
   }
   item->name = CopyString(name);
   item->action.type = type;
   item->action.data.i = value;
   item->iconName = NULL;
   item->submenu = NULL;

   item->next = menu->items;
   menu->items = item;

}

/** Select a window for performing an action. */
void ChooseWindow(const MenuAction *action) {

   XEvent event;
   ClientNode *np;

   GrabMouseForChoose();

   for(;;) {

      WaitForEvent(&event);

      if(event.type == ButtonPress) {
         if(event.xbutton.button == Button1) {
            np = FindClientByWindow(event.xbutton.subwindow);
            if(np) {
               client = np;
               RunWindowCommand(action);
            }
         }
         break;
      } else if(event.type == KeyPress) {
         break;
      }

   }

   JXUngrabPointer(display, CurrentTime);

}

/** Window menu action callback. */
void RunWindowCommand(const MenuAction *action) {

   switch(action->type) {
   case MA_STICK:
      if(client->state.status & STAT_STICKY) {
         SetClientSticky(client, 0);
      } else {
         SetClientSticky(client, 1);
      }
      break;
   case MA_MAXIMIZE:
      MaximizeClient(client, 1, 1);
      break;
   case MA_MAXIMIZE_H:
      MaximizeClient(client, 1, 0);
      break;
   case MA_MAXIMIZE_V:
      MaximizeClient(client, 0, 1);
      break;
   case MA_MINIMIZE:
      MinimizeClient(client);
      break;
   case MA_RESTORE:
      RestoreClient(client, 1);
      break;
   case MA_CLOSE:
      DeleteClient(client);
      break;
   case MA_SENDTO:
   case MA_DESKTOP:
      SetClientDesktop(client, action->data.i);
      break;
   case MA_SHADE:
      if(client->state.status & STAT_SHADED) {
         UnshadeClient(client);
      } else {
         ShadeClient(client);
      }
      break;
   case MA_MOVE:
      MoveClientKeyboard(client);
      break;
   case MA_RESIZE:
      ResizeClientKeyboard(client);
      break;
   case MA_KILL:
      KillClient(client);
      break;
   case MA_LAYER:
      SetClientLayer(client, action->data.i);
      break;
   default:
      Debug("unknown window command: %d", action->type);
      break;
   }

}

