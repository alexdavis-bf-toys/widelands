dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "mushroom_green_wasteland_mature",
   descname = _ "Green Mushroom Tree (Mature)",
   editor_category = "trees_wasteland",
   size = "small",
   attributes = {},
   programs = {
      program = {
         "animate=idle 72000",
         "remove=19",
         "grow=mushroom_green_wasteland_old",
      },
   },
   animations = {
      idle = {
         pictures = path.list_directory(dirname, "idle_\\d+.png"),
         hotspot = { 18, 48 },
         fps = 8,
      },
   },
}
