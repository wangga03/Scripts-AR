[System.Serializable]
public class ButtonState
{
    public bool state;

    public ButtonState(bool state)
    {
        this.state = state;
    }

}

[System.Serializable]
public class IncrementValue
{
    public int value;

    public IncrementValue(int value)
    {
        this.value = value;
    }
}

