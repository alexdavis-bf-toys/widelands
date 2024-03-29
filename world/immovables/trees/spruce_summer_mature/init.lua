dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "spruce_summer_mature",
   descname = _ "Spruce (Mature)",
   editor_category = "trees_coniferous",
   size = "small",
   attributes = {},
   programs = {
      program = {
         "animate=idle 60000",
         "remove=23",
         "grow=spruce_summer_old",
      },
   },
   animations = {
      idle = {
         pictures = path.list_directory(dirname, "idle_\\d+.png"),
         hotspot = { 12, 48 },
         fps = 8,
      },
   },
}
