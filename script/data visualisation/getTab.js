var items=[], options=[];
var i = 0
//Iterate all td's in second column
$('.table.table-condensed.table-striped tr ').each( function(){
   if ( i % 2 === 0){
	   //add item to array
	   year = $(this).find('td span').first().text()
	   year= year.split('/')[1]
	   peak = $(this).find('td:nth-child(7)').text()
	   console.log(year + "  " + peak)
	}
		   i = i + 1

});
