import React, { Component, PropTypes } from 'react';
import Box from 'grommet/components/Box';



export default class Proc extends Component {

  constructor () {
    super();
    this.state = {
    };
  }

  render () {

    var width_stick = (100-(this.props.nb_core)/2) / (this.props.nb_core);
    var core_sticks ;
    core_sticks = [];
    for(var i = 0; i<this.props.nb_core; i++){
      core_sticks.push(
        <div className="equalizer-stick-container" id={"core_" + i} style={{width: width_stick+"%"}}>
          <div className="equalizer-stick"/>
          <div className="equalizer-line" />
        </div>
      );
    }

    return (
      <Box direction="row" colorIndex="accent-2"  >
        <div className="socket" id={"socket_" + this.props.numero }>




          <div className="temp-stick-container" id={"temp_socket_" + this.props.numero}>
            <div className="temp-stick"> 30 </div>
            <div className="temp-line" > </div>
          </div>
          <img  src={require('./pictures/temperature.png')} alt="Temperature" className="icon_temperature" />


          <div className="power-stick-container" id={"power_socket_" + this.props.numero}>
            <div className="power-stick"> 30 </div>
            <div className="power-line" > </div>
          </div>
          <img  src={require('./pictures/power.png')} alt="Power" className="icon_power" />

          <span className="echelle_indicateur-0_0 indicateur"> 0% </span>
          <span className="echelle_indicateur-0_5 indicateur"> 50% </span>
          <span className="echelle_indicateur-1_0 indicateur"> 100% </span>
          <span className="echelle_indicateur-1_5 indicateur"> 150% </span>
          <span className="echelle_indicateur-2_0 indicateur"> 200% </span>

          <div className="equalizer">
            <div id="echelle">
              <div id="echelle-1_5"></div>
              <div id="echelle-1_0"></div>
              <div id="echelle-0_5"></div>
              <div id="echelle-0_0"></div>
            </div>
            {core_sticks}
          </div>
          <img  src={require('./pictures/processor.png')} alt="Processor" className="icon_proc" />
        </div>
      </Box>
    );
  }
};


Proc.propTypes = {
  numero: PropTypes.number.isRequired,
  nb_core: PropTypes.number.isRequired
}
