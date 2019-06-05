This skin has just hotcues instead of the loop_buttons.
The hotcues buttons is changed from smallSquareButton to squareButton.
(remember: all the buttons sizes are defined in skin.xml)

The logic is always that the click on the button is for delete the hotcue.
You probably want to change this beheavure 
to make the hotcue being setted/ start when is pressed with left click.
To do this change change in hotcue_buttons.xml lines 23-25 

from

    <SetVariable name="right_connection_control"><Variable name="group"/>,hotcue_<Variable name="hotcue"/>_activate</SetVariable>
    <SetVariable name="left_connection_control"><Variable name="group"/>,hotcue_<Variable name="hotcue"/>_clear</SetVariable>

to

	<SetVariable name="right_connection_control"><Variable name="group"/>,hotcue_<Variable name="hotcue"/>_clear</SetVariable>
    <SetVariable name="left_connection_control"><Variable name="group"/>,hotcue_<Variable name="hotcue"/>_activate</SetVariable>


