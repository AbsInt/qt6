# Generated from qtwaylandscanner.pro.

#####################################################################
## qtwaylandscanner Tool:
#####################################################################

qt_get_tool_target_name(target_name qtwaylandscanner)
qt_internal_add_tool(${target_name}
    TOOLS_TARGET WaylandScanner # special case
    SOURCES
        qtwaylandscanner.cpp
)

#### Keys ignored in scope 1:.:.:qtwaylandscanner.pro:<TRUE>:
# _OPTION = "host_build"

# special case begin
# Abuse the function to make sure the package providing qtwaylandscanner calls
# find_package(WaylandScanner), aka the non-qt provided package.
qt_record_extra_package_dependency(qtwaylandscanner WaylandScanner "")
# special case end
