template <typename NodeContents, typename Criteria> class pattern
{
};

/*
template <size_t ExpressionFrameLength>
class pattern : pattern<expression<ExpressionFrameLength>>
{
    //A pattern is a sequence of phrases.
    //A pattern should contain 0-n expressions. 
    //It should have a method to select and play an expression. 
    //This method should select what to play on the basis of game heuristics.
    //Selectable methods should be able to:
    //  a) maintain internal position and play vector members in a loop
    //  b) jump to specific members if appropriate, including the one that just played
    //  c) point to another pattern and access it instead.
    //Patterns could be used for:
    //  a) rhythms - e.g. there are 4 different drum tracks and 4 different fills, and one is just selected to loop back to itself until some transition, when a fill is selected, etc.
    //  b) continuous melodies - iterate across them until they loop, then go to something else or repeat
    //  c) patterns of patterns. Since patterns can "play" another pattern, they can be used to step up a directed graph of patterns to play, so we can use patterns to time (e.g. "4 cycles" or "end of this vector") transitions among song elements.
    //
    //
    //I think a class along these lines could be the backbone of the song structure... or, more likely, subclasses of it for specific reusable things in songs like drum track or counterpoint line or whatever

};

*/


