/*
 * Copyright (C) 2010-2015 Nektra S.A., Buenos Aires, Argentina.
 * All rights reserved. Contact: http://www.nektra.com
 *
 *
 * This file is part of Deviare
 *
 *
 * Commercial License Usage
 * ------------------------
 * Licensees holding valid commercial Deviare licenses may use this file
 * in accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Nektra.  For licensing terms and
 * conditions see http://www.nektra.com/licensing/. Use the contact form
 * at http://www.nektra.com/contact/ for further information.
 *
 *
 * GNU General Public License Usage
 * --------------------------------
 * Alternatively, this file may be used under the terms of the GNU General
 * Public License version 3.0 as published by the Free Software Foundation
 * and appearing in the file LICENSE.GPL included in the packaging of this
 * file.  Please visit http://www.gnu.org/copyleft/gpl.html and review the
 * information to ensure the GNU General Public License version 3.0
 * requirements will be met.
 *
 **/

using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using CSharpConsole.Properties;
using Nektra.Deviare2;

namespace CSharpConsole.Extensions
{
    public class ProcessIcon
    {
        public static Image IconFrom(string aProcessPath)
        {
            if (!File.Exists(aProcessPath)) return Resources.BlankImage;

            try
            {
                var bigIcon = Icon.ExtractAssociatedIcon(aProcessPath);
                return Generate16X16PixelsIcon(bigIcon);
            }
            catch
            {
                return Resources.BlankImage;
            }
        }

        private static Bitmap Generate16X16PixelsIcon(Icon anIcon)
        {
            var aBitmap = new Bitmap(anIcon.ToBitmap());
            var an16X16Icon = new Bitmap(16, 16);
            var graphics = Graphics.FromImage(an16X16Icon);

            graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;
            graphics.DrawImage(aBitmap, new Rectangle(0, 0, 16, 16), new Rectangle(0, 0, aBitmap.Width, aBitmap.Height), GraphicsUnit.Pixel);

            graphics.Dispose();
            aBitmap.Dispose();

            return an16X16Icon;
        }
    }
    public static class NktProcessExtensions
    {
        public static Image Icon(this NktProcess aProcess)
        {
            return ProcessIcon.IconFrom(aProcess.Path);
        }

        
    }
}
