dirname = path.dirname(__file__)

animations = {
   idle = {
      pictures = { dirname .. "fallentree_idle.png" },
      hotspot = { 2, 31 },
   },
}

world:new_immovable_type{
   name = "fallentree",
   descname = _ "Fallen Tree",
   editor_category = "trees_dead",
   size = "none",
   attributes = {},
   programs = {
      program = {
	     "animate=idle 30000",
		 "remove="
      }
   },
   animations = animations,
}
