dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "mushroom_green_wasteland_pole",
   descname = _ "Green Mushroom Tree (Pole)",
   editor_category = "trees_wasteland",
   size = "small",
   attributes = {},
   programs = {
      program = {
         "animate=idle 65000",
         "remove=24",
         "grow=mushroom_green_wasteland_mature",
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
