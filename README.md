# Offroad Rally Rewrite
This is a rewrite of an old school project meant to compare pathfinding 
algorithms.

The Offroad Rally takes place on a square map made up of hexagons. Each hex
has a roughness value, and the cost of moving from one hex to another is the
combined roughness of the two hex. The starting and finishing point of the race
always have a roughness of one, and are graphically shown as '*' and '&'
respectively. For extra visibility, a '|' symbol is placed on top of the map
on the same column as the start, and additionally a '|' is placed on the bottom
of the map on the same column as the end.

Here is an example map:
```
 |
8 1 3 5
 * 4 5 6
  3 8 9 4
   1 5 9 &
         |
```

The pathfinding agents return a list of directions they moved in. Their paths 
are scored based on if they successfully moved from the starting point
to the finishing point, how much their path cost, and how many times they needed
to look at the map for movement costs.
