# Python Signed distance field generator

This util is used to create static distance field images. Provinces and states should not move in a game, so this makes sense, somehow. I tried to port this to GDScript, but it's too slow and complicated.

If anyone has C# knowledge and those libraries exist (or equivalent)... we're golden!

- Install Python (3?)
- Create a virtual env: python -m venv .venv
- Go into it: source .venv/bin/activate
- Install dependencies: pip install numpy scipy pillow

Then run the scripts:

```python
python states.py 
python provinces.py
```