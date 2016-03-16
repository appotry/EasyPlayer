/*****************************************************************************
 * Util.java
 *****************************************************************************
 * Copyright © 2011-2014 VLC authors and VideoLAN
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

package org.easydarwin.easypusherlive.vlc;

import android.annotation.TargetApi;
import android.content.SharedPreferences;
import android.os.Build;
import org.videolan.libvlc.util.AndroidUtil;
import java.io.Closeable;
import java.io.IOException;

public class Util {


//    private static String getMediaString(Context ctx, int id) {
//        if (ctx != null)
//            return ctx.getResources().getString(id);
//        else {
//            switch (id) {
//            case R.string.unknown_artist:
//                return "Unknown Artist";
//            case R.string.unknown_album:
//                return "Unknown Album";
//            case R.string.unknown_genre:
//                return "Unknown Genre";
//            default:
//                return "";
//            }
//        }
//    }


    @TargetApi(Build.VERSION_CODES.GINGERBREAD)
    public static void commitPreferences(SharedPreferences.Editor editor){
        if (AndroidUtil.isGingerbreadOrLater())
            editor.apply();
        else
            editor.commit();
    }



    public static boolean close(Closeable closeable) {
        if (closeable != null) {
            try {
                closeable.close();
                return true;
            } catch (IOException e) {
                return false;
            }
        } else {
            return false;
        }
    }

}
