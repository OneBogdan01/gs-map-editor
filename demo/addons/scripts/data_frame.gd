@tool
extends Resource
# from https://www.youtube.com/watch?v=Kz517iDaUtU&t=255s
class_name DataFrame


@export var data: Array
@export var columns: PackedStringArray

static func New(d: Array, c: PackedStringArray) -> DataFrame:
	var df = DataFrame.new()
	df.data = d
	
	if c:
		df.columns = c
	return df

func GetColumn(col : String):
	assert(col in columns)
	
	var ix = columns.find(col)
	var result = []
	
	for row in data:
		result.append(row[ix])
	return result
func GetRow(i: int):
	assert(i<len(data))
	
	return data[i]
	
	
func AddColumn(d: Array, col_name: String):
	assert(len(d) == len(data))
	
	for i in range(len(data)):
		data[i].append(d[i])
	columns.append(col_name)

static func EvalColumns(c1: Array, operand: String, c2: Array, convert_to_float: bool = false):
	assert(len(c1) == len(c2))
	var expression = Expression.new()
	if convert_to_float:
		expression.parse("float(a) %s float(b)" % operand, ["a", "b"])
	else:
		expression.parse("a %s b" % operand, ["a", "b"])
	
	var result = []
	
	for i in range(len(c1)):
		result.append(expression.execute(c1[i], c2[i]))
		
	return result


func _sort_by(row1, row2, ix, desc) -> bool:
	var result: bool
	
	if row1[ix] < row2[ix]:
		result = true
	else:
		result = false
		
	if desc:
		result = !result
	return result
func SortBy(col_name: String, desc: bool = false):
	assert(col_name in columns)
	
	var ix = columns.find(col_name)
	data.sort_custom(_sort_by.bind(ix, desc))
	
func _to_string():
	if len(data) == 0:
		return "empty data"
	
	
	var result = "|".join(columns) + "\n-------------\n"
	
	for row in data:
		result += " | ".join(row) + "\n"
	return result
