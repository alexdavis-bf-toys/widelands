dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "cirrus_wasteland_pole",
   descname = _ "Cirrus Tree (Pole)",
   editor_category = "trees_wasteland",
   size = "small",
   attributes = {},
   programs = {
      program = {
         "animate=idle 59000",
         "remove=34",
         "grow=cirrus_wasteland_mature",
      },
   },
   animations = {
      idle = {
         pictures = path.list_directory(dirname, "idle_\\d+.png"),
         hotspot = { 12, 28 },
         fps = 8,
      },
   },
}
