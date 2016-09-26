#include "tagtool.h"
#include "libquasd/file.h"

#include <iostream>
#include <string>
#include <locale>
#include <algorithm>

#include <glibmm-2.4/glibmm.h>


using namespace quasd;
 
tagtool::tagtool()
{
    init_tag_translation();
}

void
tagtool::append_framefilter_common(const std::string& s)
{
    // param s is a valid common tag-frame, validated by 'set_tagframefilter_common'
    // no need to check return value of get_framedef_by_common_framename
    v_filter_id3.push_back(get_framedef_by_common_framename(s, map_translate_id3));
    v_filter_m4a.push_back(get_framedef_by_common_framename(s, map_translate_m4a));
    v_filter_xiph.push_back(get_framedef_by_common_framename(s, map_translate_xiph));    
}

void
tagtool::init_tag_translation()
{   
    // class definition vector
     v_common_tagnames.push_back("album");
     v_common_tagnames.push_back("albumartist");
     v_common_tagnames.push_back("artist");
     v_common_tagnames.push_back("bpm");
     v_common_tagnames.push_back("comment");
     v_common_tagnames.push_back("genre");
     v_common_tagnames.push_back("length");
     v_common_tagnames.push_back("mood");
     v_common_tagnames.push_back("publisher");
     v_common_tagnames.push_back("rating");
     v_common_tagnames.push_back("title");
     v_common_tagnames.push_back("track");
     v_common_tagnames.push_back("year");
    
  /* ID3V2 */
    map_translate_id3.insert(std::make_pair("TALB", "album"));
    map_translate_id3.insert(std::make_pair("TPE2", "albumartist"));
    map_translate_id3.insert(std::make_pair("TPE1", "artist"));
    map_translate_id3.insert(std::make_pair("TBPM", "bpm"));
    map_translate_id3.insert(std::make_pair("COMM", "comment"));
    map_translate_id3.insert(std::make_pair("TCON", "genre"));
    map_translate_id3.insert(std::make_pair("TLEN", "length"));
    map_translate_id3.insert(std::make_pair("TMOO", "mood"));
    map_translate_id3.insert(std::make_pair("TPUB", "publisher"));
    map_translate_id3.insert(std::make_pair("POPM", "rating")); //Popularimeter
    map_translate_id3.insert(std::make_pair("TIT2", "title"));
    map_translate_id3.insert(std::make_pair("TRCK", "track"));
    map_translate_id3.insert(std::make_pair("TDRC", "year"));
    //    map_translate_id3.insert("WOAF", "Official audio file webpage");
    //    map_translate_id3.insert("WOAR", "Official artist/performer webpage");
    //    map_translate_id3.insert("WPUB", "Publishers official webpage");
    
  /* XIPH/VORBIS */
    map_translate_xiph.insert(std::make_pair("ALBUM", "album"));
    map_translate_xiph.insert(std::make_pair("ALBUMARTIST", "albumartist"));
    map_translate_xiph.insert(std::make_pair("ARTIST", "artist"));
    map_translate_xiph.insert(std::make_pair("TBPM", "bpm"));
    map_translate_xiph.insert(std::make_pair("COMMENT", "comment"));
    map_translate_xiph.insert(std::make_pair("GENRE", "genre"));
    // length
    map_translate_xiph.insert(std::make_pair("MOOD", "mood"));
    map_translate_xiph.insert(std::make_pair("PUBLISHER", "publisher"));
    map_translate_xiph.insert(std::make_pair("RATING", "rating"));
    map_translate_xiph.insert(std::make_pair("TITLE", "title"));
    map_translate_xiph.insert(std::make_pair("TRACKNUMBER", "track"));
    map_translate_xiph.insert(std::make_pair("DATE", "year"));
    
  /* Apple/m4a */
    map_translate_m4a.insert(std::make_pair("©alb", "album"));
    map_translate_m4a.insert(std::make_pair("aART", "albumartist"));
    map_translate_m4a.insert(std::make_pair("©ART", "artist"));
    map_translate_m4a.insert(std::make_pair("tmpo", "bpm"));
    map_translate_m4a.insert(std::make_pair("©cmt", "comment"));
    map_translate_m4a.insert(std::make_pair("©gen", "genre"));
    // length
    map_translate_m4a.insert(std::make_pair("----:com.apple.iTunes:MOOD", "mood"));
    map_translate_m4a.insert(std::make_pair("----:com.apple.iTunes:LABEL", "publisher"));
    // rating
    map_translate_m4a.insert(std::make_pair("©nam", "title"));
    map_translate_m4a.insert(std::make_pair("trkn", "track"));
    map_translate_m4a.insert(std::make_pair("©day", "year"));
    
    b_init_tag_translation = true;
}

bool
tagtool::is_in_filter(const std::string& frame,
                        const std::vector<std::string>& v) 
{
    for (const auto framefilter : v)
    {
        if ( frame == framefilter )
        {
            return true;
        }
    }
    return false;
}


std::string
tagtool::get_framedef_by_common_framename(const std::string& cframe,
                          const std::unordered_map<std::string, std::string>& m)
{
    for (const auto it : m)
    {
        if (it.second == cframe)
        {
            return it.first;
        }
    }
    return std::string{""};
}



bool
tagtool::parse(const std::string& f,
        std::unordered_map<std::string, std::string>& m)
{    
    // create and check Taglib Fileref
    TagLib::FileRef fileref( f.c_str() );
    if ( fileref.isNull() )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":cannot create taglib-fileref:"
                << f
                << std::endl;
        return false;
    }

    // create TagLib File according to file-extension
    std::string ext = quasd::file::get_extension(f);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
    if ( ext == ".mp3" ) 
    {
        TagLib::MPEG::File *fp = dynamic_cast<TagLib::MPEG::File *>( fileref.file() );
        if( fp->isValid() ) 
        {
            if (b_write_tagv) m.insert(std::make_pair("tagv", "id3"));
            parseID3V2Tags( fp->ID3v2Tag(), m );
            return true;
        } 
        else 
        {
            std::cerr << "ERR:" << __PRETTY_FUNCTION__
                    << ":can't read mp3 file:"
                    << f
                    << std::endl;
        }
    } 
    else if ( ext == ".flac" ) 
    {
        TagLib::FLAC::File *f = dynamic_cast<TagLib::FLAC::File *>( fileref.file() );

        if ( f->isValid() ) 
        {
            if ( f->xiphComment() )
            {
                if (b_write_tagv) m.insert(std::make_pair("tagv", "xiph"));
                parseXiphTags(f->xiphComment(), m);
                return true;
            }
            else if ( f->ID3v2Tag() )
            {
                if (b_write_tagv) m.insert(std::make_pair("tagv", "id3"));
                parseID3V2Tags(f->ID3v2Tag(), m);
                return true;
            }   
            else
            {
                std::cerr << "ERR:" << __PRETTY_FUNCTION__
                        << ":no tags found in flac-file:"
                        << f
                        << std::endl;
            }
        } 
        else 
        {
            std::cerr << "can't read flac file: " << f << std::endl;
            return false;
        }
    } 
    else if ( ext == ".ogg" ) 
    {
        TagLib::Ogg::Vorbis::File *f = 
                   dynamic_cast<TagLib::Ogg::Vorbis::File *>( fileref.file());
        
        if( f->isValid() ) 
        {
            if (b_write_tagv) m.insert(std::make_pair("xiph", "id3"));
            parseXiphTags(f->tag(), m);
            return true;
        } 
        else 
        {
            std::cerr << "ERR:" << __PRETTY_FUNCTION__
                    << ":can't read ogg-file:"
                    << f
                    << std::endl;
        }
    }
    else if ( ext == ".m4a" )
    {
        TagLib::MP4::File *f = dynamic_cast<TagLib::MP4::File *>( fileref.file());
        if( f->isValid() ) 
        {
            if (b_write_tagv) m.insert(std::make_pair("tagv", "m4a"));
            parseMP4Tags(f->tag(), m);
            return true;
        }
    }
    else if ( ext == ".wav" )
    {   
        if (b_write_tagv) m.insert(std::make_pair("tagv", "wav"));
        TagLib::RIFF::WAV::File *f = 
                dynamic_cast<TagLib::RIFF::WAV::File *>(fileref.file());
        if ( f->isValid() )
        {
            parseWavTags(f, m);
            return true;
        }
        else
        {
            std::cerr << "ERR:" << __PRETTY_FUNCTION__
                    << ":cannot read wav-file:"
                    << f
                    << std::endl;
        }
    }
    else 
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":not supported files:"
                << f
                << std::endl;
    }

    return false;
}

void
tagtool::parseID3V2Tags(TagLib::ID3v2::Tag* t,
        std::unordered_map<std::string, std::string>& m)
{
    std::string frame;
    for (const auto it : t->frameList())
    {
        TagLib::String tframe = it->frameID();
        frame = tframe.to8Bit(true);
        
        
        if ( b_filter_id3 && !is_in_filter(frame, v_filter_id3) )
        {
            continue;
        }
        
        if ( b_translate )
        {
            translate_frame(frame, map_translate_id3);
        }
        
        m.insert(std::make_pair(frame, it->toString().to8Bit(true)));
    }
}

void
tagtool::parseMP4Tags(TagLib::MP4::Tag* t,
                        std::unordered_map<std::string, std::string>& m)
{
    std::string frame;
    for (const auto it : t->itemListMap())
    {
        frame = it.first.to8Bit(true);
        
        if ( b_filter_m4a && !is_in_filter(frame, v_filter_m4a) )
        {
            continue;
        }
        
        if ( b_translate )
        {
            translate_frame(frame, map_translate_m4a);
        }
        
        // extract the value
        std::stringstream ss;
        for (auto tstr : it.second.toStringList() )
        {
            ss << tstr.to8Bit(true);
        }
        m.insert(std::make_pair(frame, ss.str()));
    }
}

void
tagtool::parseXiphTags(TagLib::Ogg::XiphComment* t,
                        std::unordered_map<std::string, std::string>& m)
{
    std::string frame;
    for (const auto it : t->fieldListMap())
    {
        frame = it.first.to8Bit(true);
        
        if ( b_filter_xiph && !is_in_filter(frame, v_filter_xiph) )
        {
            continue;
        }
        if ( b_translate )
        {
            translate_frame(frame, map_translate_xiph);
        }
        
        m.insert(std::make_pair(frame, it.second.toString(";").to8Bit(true)));
    }
}

void
tagtool::parseWavTags(TagLib::RIFF::WAV::File *t,
                        std::unordered_map<std::string, std::string>& m)
{
    if ( t->hasID3v2Tag() )
    {
        parseID3V2Tags(t->ID3v2Tag(), m);
    }
    else if ( t->hasInfoTag() )
    {
        // TODO
    }
    else
    {
        // TODO: parse filename for info???
    }
}

void
tagtool::parseAiffTags(TagLib::RIFF::AIFF::File *t,
                        std::unordered_map<std::string, std::string>& m)
{
    // TagLib::RIFF::AIFF::File::hasID3v2Tag() !not implemented yet!
    // ubuntu14.04:libtag1-dev
    // pkg-config --version libtag:0.26
    
    
    // TODO: parse filename for info???
}


void tagtool::set_tagframefilter_common(const std::vector<std::string>& v)
{
    // turn on the switches for the parse-methods
    b_translate   = true;
    b_filter_id3  = true;
    b_filter_m4a  = true;
    b_filter_xiph = true;
        
    // process user vector    
    for (const auto frame : v)
    {
        if ( std::find(v_common_tagnames.begin(), v_common_tagnames.end(),
                        frame) != v_common_tagnames.end() 
           )
        {
            append_framefilter_common(frame);
        }
        else
        {
            std::cerr << "ERR:" << __PRETTY_FUNCTION__
                    << ":not supported:"
                    << frame
                    << std::endl;
        }
    }
}


void 
tagtool::set_tagframefilter_id3(const std::vector<std::string>& v) 
{
    v_filter_id3 = v;
    b_filter_id3 = true;
}

void tagtool::set_translate(bool b)
{
    b_translate = b;
}


void
tagtool::translate_frame(std::string& s,
                           std::unordered_map<std::string, std::string>& m) 
{
    std::string t = m[s];
    if ( t.size() > 0 )
    {
        s = t;
    }
}

