Internationalization of the Matroska Shell Extension
Last Update: $Id: readme.txt,v 1.2 2003/11/19 04:50:46 jcsston Exp $
------------------------------------------------------------------

I'm using gettext to manage the translation / internationalization of the
Matroska Shell Extension.
This archive has the template file.

For the .po language files I would recommend a nice GUI editor called poEdit.
I tried it out myself and it's quite nice :)
http://sourceforge.net/projects/poedit/

You can use any text editor if you wish, Just look at "Dummy String" in the
.po to see how to enter translations.


Please note that the translation doesn't have to be complete before it will
work. English will be used for any strings/phrase not translated.

To install a translation you first need to convert the .po to a .mo, poEditr 
does that automaticly.
Then create a folder called 'lang' where you installed MatroskaProp
Under 'lang' you create a folder the name of the language you translated to.
Under that make a folder called 'LC_MESSAGES'.
Finally copy the .mo file to 'LC_MESSAGES' and rename the .mo to 'MatroskaProp.mo'

Your end result should be something like: 
'C:\program files\matroskaprop\lang\Klingon\LC_MESSAGES\MatroskaProp.mo'

When you are done with the translation and wish it to be inlcuded in the offical
installer just .zip the .po file and e-mail it to me at mse@jory.cjb.net
