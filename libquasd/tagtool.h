#ifndef TAGPARSER_H
#define TAGPARSER_H

#include <string>
#include <unordered_map>

#include <taglib/tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v1tag.h>
#include <taglib/xiphcomment.h>
#include <taglib/id3v2frame.h>
#include <taglib/textidentificationframe.h>
#include <taglib/tbytevector.h>
#include <taglib/tstring.h>
#include <taglib/tlist.h>
#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/flacfile.h>
#include <taglib/oggfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/mp4file.h>
#include <taglib/wavfile.h>
#include <taglib/aifffile.h>

namespace quasd
{
class tagtool
{
    bool b_write_tagv = false; // write tag-version?
    bool b_filter_id3 = false; // search only for id3-tags in v_filter_id3
    std::vector<std::string> v_filter_id3;
    bool b_filter_m4a = false;
    std::vector<std::string> v_filter_m4a;
    bool b_filter_xiph = false;
    std::vector<std::string> v_filter_xiph;
    
    bool b_filter_common = false; // filter all tag-types by common-name
    std::vector<std::string> v_filter_common;
    
    // definitions: "const" after init_tag_translation
    std::vector<std::string> v_common_tagnames;
    std::unordered_map<std::string, std::string> map_translate_id3;
    std::unordered_map<std::string, std::string> map_translate_xiph;
    std::unordered_map<std::string, std::string> map_translate_m4a;
    
    bool b_translate = false;
    bool b_init_tag_translation = false;
public:
    tagtool();
    
    /**
     * @brief Returns true if param1 frame is in param2 vector.
     * @param std::string frame
     * @param std::vector<std::string>
     * @return bool
     */
    bool is_in_filter(const std::string& frame,
                      const std::vector<std::string>& v);

    bool parse(const std::string& f, 
                      std::unordered_map<std::string, std::string>& m);
    
    /**
     * @brief If used, parse will return a map containing only the tags that
     * are specified in v:
     * e.g. TPE2, TIT2, TALB, etc.
     * @param const std::vector<std::string>& v
     */
    void set_tagframefilter_id3(const std::vector<std::string>& v);
    
    /**
     * @brief If used, parse will return a map containing only the tag-frames that
     * are specified in v for all supported tag-formats. See set_translate(bool) for
     * details.
     * 
     * @param const std::vector<std::string>& v
     */
    void set_tagframefilter_common(const std::vector<std::string>& v);
    
    /**
     * @brief If set to true the method parse will translate the tag-frames to a
     * readable string. Supported strings are:
     * 
     * artist
     * albumartist
     * title
     * track
     * length
     * album
     * genre
     * publisher
     * year
     * bpm
     * rating
     * mood
     * comment
     * 
     * @param b
     */
    void set_translate(bool b);
    
    /**
     * @brief If set to true, the method parser will add a pair to the map
     * describing which kind of tag was parsed:
     * tagv=id3, tagv=xiph, tagv=m4a
     * 
     * @param bool
     */
    void set_write_tagv(bool b) { b_write_tagv = b; };


private:
    /**
     * @brief Used by 
     * @param s
     */
    void append_framefilter_common(const std::string& s);
    /**
     * @brief Write, if found in param2 m, the translated frame to param1
     * @param std::string frame
     * @param std::unordered_map<std::string, std::string>& (translation-definition)
     */
    
    /**
     * @brief called by set_translate
     */
    void init_tag_translation();

    /**
     * @brief Get the tag-frame-definition for param1 cframe from param2 m.
     * 
     * e.g {"album", map_translate_id3} returns "TALB"
     * 
     * @param const std::string& cframe
     * @param const std::unordered_map<std::string, std::string>& m
     * @return std::string frame
     */
    std::string get_framedef_by_common_framename(const std::string& cframe,
                         const std::unordered_map<std::string, std::string>& m);
    
    void translate_frame(std::string& s, std::unordered_map<std::string, std::string>& m);
    
    void parseID3V2Tags(TagLib::ID3v2::Tag *t,
                               std::unordered_map<std::string, std::string>& m);
    void parseXiphTags(TagLib::Ogg::XiphComment *t,
                              std::unordered_map<std::string, std::string>& m);
    void parseMP4Tags(TagLib::MP4::Tag *t,
                             std::unordered_map<std::string, std::string>& m);
    void parseWavTags(TagLib::RIFF::WAV::File *t,
                             std::unordered_map<std::string, std::string>& m);
    void parseAiffTags(TagLib::RIFF::AIFF::File *t,
                             std::unordered_map<std::string, std::string>& m);
};

} // namespace quasd
 
#endif // TAGPARSER_H
