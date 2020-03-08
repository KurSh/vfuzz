# Add rule

### Semantics

### Format

An add rule is a string consisting of 1, 2 or 3 components.


The first components denotes the constraint set.

The second component, if given, denotes the selector set.

The third component, if given, denotes the rewrite rule set.

The components are separated by the pipe character ```|```.


So the add rule has the format:


```constraintset|selectorset|rewriteruleset```


## Constraint set

### Semantics

### Format

A constraint set consists of a comma-separated set of constraints.


A constraint optionally starts with an exclamation mark ```!```. This denotes inversion.

The following character is either an ```s``` or a ```g```. This denotes Sensor and Generator, respectively.

The remainder of the characters must be numeric and denote the Sensor ID or Generator ID.

### Examples

```!s1``` - Not Sensor 1

```g2``` - Generator 2

```!s1,g2``` - Not Sensor 1, Generator 2


## Selector

### Semantics

### Format

A selector set consists of a comma-separated set of selectors.


Each selector consists of 1 or more numeric characters that denote a Generator ID.

### Examples

```1``` - Generator 1

```2``` - Generator 2

```1,2``` - Generator 1, Generator 2

## Rewrite rule

### Semantics

### Format

A rewrite rule set consists of a comma-separated set of rewrite rules.


Each rewrite rule consists of 1 or more numeric characters that denote a Generator ID,

followed by the equal sign ```=```,

followed by 1 or more numeric characters that denote a Generator ID.


### Examples

```1=2``` - Generator 1 to Generator 2

```2=3``` - Generator 2 to Generator 3

```1=2,2=3``` - Generator 1 to Generator 2, Generator 2 to Generator 3

