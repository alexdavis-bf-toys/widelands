dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "palm_date_desert_old",
   descname = _ "Date Palm (Old)",
   editor_category = "trees_palm",
   size = "small",
   attributes = { "tree" },
   programs = {
      program = {
         "animate=idle 1050000",
         "transform=deadtree5 32",
         "seed=palm_date_desert_sapling",
      },
      fall = {
         "remove=",
      },
   },
   animations = {
      idle = {
         pictures = path.list_directory(dirname, "idle_\\d+.png"),
         hotspot = { 24, 60 },
         fps = 10,
         sound_effect = {
            directory = "sound/animals",
            name = "bird1",
         },
      },
   },
}
