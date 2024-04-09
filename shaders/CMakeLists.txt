configure_file("particle.vert" "particle.vert" COPYONLY)
configure_file("particle.frag" "particle.frag" COPYONLY)

configure_file("gravity.comp" "gravity.comp" COPYONLY)
configure_file("particles_cells.comp" "particles_cells.comp" COPYONLY)
configure_file("prefix_sum_cells.comp" "prefix_sum_cells.comp" COPYONLY)
configure_file("sort_particles.comp" "sort_particles.comp" COPYONLY)
configure_file("position_solver.comp" "position_solver.comp" COPYONLY)
configure_file("correct_velocity.comp" "correct_velocity.comp" COPYONLY)