import React from 'react';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import InputRange from 'react-input-range';
import { faToggleOff, faToggleOn, faLightbulb, faFan, faHeat, faOutlet, faCarBattery, faMapMarked, faLocation } from '@fortawesome/pro-light-svg-icons';

import './DeviceDetails.scss';

const icons = {
    'lightbulb': faLightbulb,
    'fan': faFan,
    'heat': faHeat,
    'outlet': faOutlet,
    'car-battery': faCarBattery,
    'map-marked': faMapMarked,
    'location': faLocation,
};

const formatString = string => {
    string = string.replace(/([A-Z])/g, ' $1').trim();
    return string.charAt(0).toUpperCase() + string.slice(1);
}

export default (props) => {
    const capabilities = Object.entries(props.capabilities).map(arr => {
        const type = arr[1].split(':');
        switch(type[0]) {
            case 'percent':
                return (
                    <span key={arr[0]} className='percent'>
                        <InputRange
                            value={props.state[arr[0]]}
                            minValue={0}
                            maxValue={255}
                            onChange={value => props.setDeviceState(props.id, arr[0], {value:value})}
                            />
                    </span>
                );
            case 'number':
                return (
                    <span key={arr[0]} className='percent'>
                        <InputRange
                            value={props.state[arr[0]]}
                            minValue={type[1]}
                            maxValue={type[2]}
                            onChange={value => props.setDeviceState(props.id, arr[0], {value:value})}
                            />
                    </span>
                );
        case 'bool':
                return (
                    <FontAwesomeIcon 
                        key={arr[0]} 
                        className={props.state[arr[0]] ? 'toggle on' : 'toggle'} 
                        icon={props.state[arr[0]] ? faToggleOn : faToggleOff}
                        onClick={() => props.setDeviceState(props.id, arr[0], {value:props.state[arr[0]] ? false : true})}
                        />
                );
            default:
                return null;
        }
    }).filter(x => x);
    const state = Object.entries(props.state).map(arr => {
        if (props.capabilities[arr[0]])
            return null;
        else
            return (<span>{formatString(arr[0])}: {arr[1]}</span>);
    }).filter(x => x);

    return (
        <div className='row device-details align-items-center'>
            <div className='col col-icon'><FontAwesomeIcon className='icon fa' icon={icons[props.icon]} /></div>
            <div className='col-2'>{props.name}</div>
            <div className='col'>
                <div className='d-flex align-items-center capabilities'>
                    {capabilities}
                    {state}
                </div>
            </div>
        </div>
    );
};