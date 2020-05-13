"""
Script for the TestBehavior class
"""
# import base_behavior
# from PythonQt.components import Transform
import sys
# sys.path.append("C:/Users/dante/Documents/Projects/grand-blue-engine/third_party/Python38-32/Lib/site-packages") 
import numpy as np

# ==============================================================================
# Globals
# ==============================================================================

# ==============================================================================
# Classes
# ==============================================================================

class SpinBehavior(BaseBehavior):
    """ Spin something
    """
    def __init__(self, scene_object):
        """Create the behavior, and underlying C++ PythonBehavior object"""
        BaseBehavior.__init__(self, scene_object)
        return

    def initialize(self):
        """Initialize"""
        BaseBehavior.initialize(self)
        self.total_ms = 0

        return 2


    def update(self, delta_ms: int):
        BaseBehavior.update(self, delta_ms)

        self.total_ms += delta_ms
        effects_shader = self.resources.get_shader("effects")
        rim_color = effects_shader.get_uniform("rimColor")
        new_color = [(1 + np.sin(i*self.total_ms/1000.0))/2.0
            for i, c in enumerate(rim_color)]
        effects_shader.set_uniform("rimColor", new_color)
        # effects_shader.set_uniform("rimColor", [0, 1, 0])

        # Test engine access
        pos = self.transform.translation().get_position()
        held_up = self.input.key_handler().is_held("up")
        held_right = self.input.key_handler().is_held("right")
        held_left = self.input.key_handler().is_held("left")
        held_down = self.input.key_handler().is_held("down")
        if held_up:
            print("Held up!")
            self.transform.translation().set_position(
                pos + Vector3([0, 0, 0.1]))
            pos = self.transform.translation().get_position()
        if held_down:
            print("Held down!")
            self.transform.translation().set_position(pos +
             Vector3([0, 0, -0.1]))
            pos = self.transform.translation().get_position()
        if held_right:
            print("Held right!")
            self.transform.translation().set_position(pos +
             Vector3([0.1, 0, 0.0]))
            pos = self.transform.translation().get_position()
        if held_left:
            print("Held left!")
            self.transform.translation().set_position(pos +
             Vector3([-0.1, 0, 0.0]))
            pos = self.transform.translation().get_position()

        if self.input.mouse_handler().moved():
            # print(self.input.mouse_handler().screen_pos())
            pass

        return 2

    def late_update(self, delta_ms: int):
        # Camera operations go here
        BaseBehavior.late_update(self, delta_ms)
        # print("Performing late update")
        return 2

    def fixed_update(self, delta_ms: int):
        BaseBehavior.fixed_update(self, delta_ms)

        # Set rotation
        rotation = EulerAngles(0.0, 0.0005, 0.0, [0, 1, 2], 0)
        self.transform.rotation().add_rotation(rotation)


        # Test access to engine
        e = self.engine
        sc = e.scenario()

        return 2