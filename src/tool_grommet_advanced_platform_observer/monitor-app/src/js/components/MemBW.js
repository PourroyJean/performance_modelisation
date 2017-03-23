import React, { Component, PropTypes } from 'react';
import Box from 'grommet/components/Box';



export default class MemBW extends Component {

  constructor () {
    super();
    this.state = {
    };
  }

  render () {


    var styles = {
      arrow_down_1: {
        top:   (0*(12.5))+"%", backgroundColor: "#99f7ff"
      },
      arrow_down_2: {
        top:   (1*(12.5))+"%", backgroundColor: "#36a2eb"
      },
      arrow_down_3: {
        top:   (2*(12.5))+"%", backgroundColor: "#99f7ff"
      },
      arrow_down_4: {
        top:   (3*(12.5))+"%", backgroundColor: "#36a2eb"
      },
      arrow_down_5: {
        top:   (4*(12.5))+"%", backgroundColor: "#99f7ff"
      },
      arrow_down_6: {
        top:   (5*(12.5))+"%", backgroundColor: "#36a2eb"
      },
      arrow_down_7: {
        top:   (6*(12.5))+"%", backgroundColor: "#99f7ff"
      },
      arrow_down_8: {
        top:   (7*(12.5))+"%", backgroundColor: "#36a2eb"
      },
      arrow_up_1: {
        bottom:   (0*(12.5))+"%", backgroundColor: "#99f7ff"
      },
      arrow_up_2: {
        bottom:   (1*(12.5))+"%", backgroundColor: "#36a2eb"
      },
      arrow_up_3: {
        bottom:   (2*(12.5))+"%", backgroundColor: "#99f7ff"
      },
      arrow_up_4: {
        bottom:   (3*(12.5))+"%", backgroundColor: "#36a2eb"
      },
      arrow_up_5: {
        bottom:   (4*(12.5))+"%", backgroundColor: "#99f7ff"
      },
      arrow_up_6: {
        bottom:   (5*(12.5))+"%", backgroundColor: "#36a2eb"
      },
      arrow_up_7: {
        bottom:   (6*(12.5))+"%", backgroundColor: "#99f7ff"
      },
      arrow_up_8: {
        bottom:   (7*(12.5))+"%", backgroundColor: "#36a2eb"
      }
    }


    return (
      <Box direction="row" colorIndex="warning"  className="membw-container" id={"membw_socket_" + this.props.numero} >

        <div className="membw-arrow-container">;


          <img src={require('./pictures/new-white-arrow-down.png')} alt="membw" className="icon-membw-arrow-down" />
          <img src={require('./pictures/new-white-arrow-up.png')}  alt="membw" className="icon-membw-arrow-up" />
          <img src={require('./pictures/white-ram.png')}  alt="RAM" className="icon-ram" />
          <div className="membw-stick-container membw-stick-container-down">
            <div className="membw-echelle">
              <div className="membw-echelle-number-1" style={styles.arrow_down_1}> </div>
              <div className="membw-echelle-number-2" style={styles.arrow_down_2}> </div>
              <div className="membw-echelle-number-3" style={styles.arrow_down_3}> </div>
              <div className="membw-echelle-number-4" style={styles.arrow_down_4}> </div>
              <div className="membw-echelle-number-5" style={styles.arrow_down_5}> </div>
              <div className="membw-echelle-number-6" style={styles.arrow_down_6}> </div>
              <div className="membw-echelle-number-7" style={styles.arrow_down_7}> </div>
              <div className="membw-echelle-number-8" style={styles.arrow_down_8}> </div>
            </div>
            <div className="membw-stick membw-stick-down"> </div>
            <div className="membw-line membw-line-down"  > </div>
          </div>
          <div className="membw-stick-container membw-stick-container-up">
            <div className="membw-echelle">
              <div className="membw-echelle-number-1" style={styles.arrow_up_1}> </div>
              <div className="membw-echelle-number-2" style={styles.arrow_up_2}> </div>
              <div className="membw-echelle-number-3" style={styles.arrow_up_3}> </div>
              <div className="membw-echelle-number-4" style={styles.arrow_up_4}> </div>
              <div className="membw-echelle-number-5" style={styles.arrow_up_5}> </div>
              <div className="membw-echelle-number-6" style={styles.arrow_up_6}> </div>
              <div className="membw-echelle-number-7" style={styles.arrow_up_7}> </div>
              <div className="membw-echelle-number-8" style={styles.arrow_up_8}> </div>
            </div>
            <div className="membw-stick membw-stick-up"> </div>
            <div className="membw-line membw-line-up"  > </div>
          </div>
        </div>
        <div className="membw-counter-max-down"><div className="title-membw">Max </div><div className="number-membw"> </div><div className="unit-membw">GB/s </div> </div>
        <div className="membw-counter-avg-down"><div className="title-membw">Avg</div><div className="number-membw"> </div><div className="unit-membw">GB/s </div> </div>
        <div className="membw-counter-max-up"><div className="title-membw">Max</div><div className="number-membw"> </div><div className="unit-membw">GB/s </div> </div>
        <div className="membw-counter-avg-up"><div className="title-membw">Avg</div><div className="number-membw"> </div><div className="unit-membw">GB/s </div> </div>
        <div className="membw-counter-max-sum"> <div className="title-membw">Max </div><div className="number-membw"> </div><div className="unit-membw">GB/s </div> </div>


      </Box>
    );
  }
};


MemBW.propTypes = {
  numero: PropTypes.number.isRequired
}
