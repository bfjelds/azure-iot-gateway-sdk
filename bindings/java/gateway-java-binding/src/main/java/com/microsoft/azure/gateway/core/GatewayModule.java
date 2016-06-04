/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */
package com.microsoft.azure.gateway.core;

import com.microsoft.azure.gateway.messaging.Message;

/**
 * The Abstract Module class to be extended by the module-creator when creating any modules.
 */
public abstract class GatewayModule implements IGatewayModule{

    public abstract void receive(Message message);
    public abstract void destroy();

    /** The address of the native module pointer. */
    private long _addr;

    /** The {@link MessageBus} to which this module belongs */
    private MessageBus bus;

    /** The module-specific configuration object. */
    private String configuration;

    /**
     * Constructs a {@link GatewayModule} from the provided address and {@link MessageBus}. A {@link GatewayModule} should always call this super
     * constructor before any module-specific constructor code.
     *
     * @param address The address of the native module pointer
     * @param bus The {@link MessageBus} to which this module belongs
     * @param configuration The module-specific configuration
     */
    public GatewayModule(long address, MessageBus bus, String configuration){
        this.create(address, bus, configuration);
    }

    public void create(long moduleAddr, MessageBus bus, String configuration){
        this._addr = moduleAddr;
        this.bus = bus;
        this.configuration = configuration;
    }

    public void receive(byte[] serializedMessage){
        this.receive(new Message(serializedMessage));
    }

    //Public getter methods
    final public long getAddress(){
        return _addr;
    }

    final public MessageBus getMessageBus(){
        return bus;
    }

    final public String getConfiguration(){
        return configuration;
    }
}