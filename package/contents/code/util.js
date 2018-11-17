/*
 *   Copyright 2017 by Fabian Vogt <fabian@ritter-vogt.de>
 *   Copyright 2017 by Kai Uwe Broulik <kde@privat.broulik.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

.pragma library

/* Like QString::toHtmlEscaped */
function toHtmlEscaped(s) {
    return s.replace(/[&<>]/g, function (tag) {
        return {
            '&': '&amp;',
            '<': '&lt;',
            '>': '&gt;'
        }[tag] || tag
    });
}

function underlineAmpersands(match, p) {
    if(p == "&amp;")
        return p;

    return "<u>" + p + "</u>";
}

/* This function is a replacement for the flawed
 * QtQuickControlsPrivate.StyleHelpers.stylizeMnemonics.
 * It scans the passed text for mnemonics, to put them into HTML <u></u>
 * tags. This means it emits HTML, but accepts only plaintext.
 * Simply passing HTML escaped plaintext won't work, as it would then
 * replace &lt; with <u>l</u>t; so we need to implement it ourselves. */
function stylizeEscapedMnemonics(text) {
	return text.replace(/&amp;(&amp;|.)/g, underlineAmpersands);
} 
